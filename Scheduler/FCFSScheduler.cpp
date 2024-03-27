/**
 * @file    FCFSScheduler.cpp
 *
 * @brief   First Come First Serve Scheduling
 */

#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include "JobTrace.h" // Your JobTrace structure needs to be defined

std::queue<JobTrace> jobQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;
bool finishedReceiving = false;
int totalExecutionTime = 0;
int totalWaitTime = 0;
int totalLatency = 0;
int totalJobsProcessed = 0;

void jobReceiver(int client_sock) {
    while (true) {
        JobTrace job;
        ssize_t read_size = recv(client_sock, &job, sizeof(job), 0);
        if (read_size > 0) {
            // std::cout << "received a job from client" << std::endl;
            // std::cout << "Job ID: " << job.jobID << std::endl;
            std::lock_guard<std::mutex> lock(queueMutex);
            job.qRecvTime = std::chrono::system_clock::now();
            jobQueue.push(job);
            queueCondition.notify_one(); // Notify the processor thread
        } else {
            if (read_size == 0) {
                std::cout << "Client disconnected." << std::endl;
            } else if (read_size == -1) {
                std::cerr << "Recv failed." << std::endl;
            }
            break;
        }
    }
    finishedReceiving = true;
    queueCondition.notify_all(); // Ensure the processor thread can exit if waiting
}

void jobProcessor() {
    std::unique_lock<std::mutex> lock(queueMutex, std::defer_lock);
    std::ofstream outFile("Results/fcfs_scheduler_results.csv", std::ios_base::app); // Open the output file in append mode
    std::vector<int> latencies; // Store wait times for all jobs
    double arrivalRate;

    while (!finishedReceiving || !jobQueue.empty()) {
        lock.lock();
        queueCondition.wait(lock, [] { return finishedReceiving || !jobQueue.empty(); });

        if (!jobQueue.empty()) {
            JobTrace job = jobQueue.front();
            jobQueue.pop();
            lock.unlock();

            auto startProcessingTime = std::chrono::system_clock::now();
            auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(startProcessingTime - job.qRecvTime).count();

            // Update total wait time
            totalWaitTime += waitTime;

            std::this_thread::sleep_for(std::chrono::milliseconds(job.jobSize)); // Simulate processing

            auto endProcessingTime = std::chrono::system_clock::now();
            auto endToEndLatency = std::chrono::duration_cast<std::chrono::milliseconds>(endProcessingTime - job.qRecvTime).count();
            
            // Add each latency to the vector
            latencies.push_back(endToEndLatency);

            // Update total latency
            totalLatency += endToEndLatency;

            // Update total execution time
            totalExecutionTime += job.jobSize;

            // outFile << "Processed job " << job.jobID 
            //         << " of size " << job.jobSize
            //         << " with wait time " << waitTime << " milliseconds"
            //         << ", end-to-end latency " << endToEndLatency << " milliseconds." << "\n";
            // outFile.flush();

            // Increment total jobs processed
            totalJobsProcessed++;
            std::cout << "total jobs processed: " << totalJobsProcessed << std::endl;
            arrivalRate = job.arrivalRate;
        } else {
            lock.unlock();
        }
    }

    // Calculate average wait time and average latency
    double averageWaitTime = totalWaitTime / static_cast<double>(totalJobsProcessed);
    double averageLatency = totalLatency / static_cast<double>(totalJobsProcessed);

    std::sort(latencies.begin(), latencies.end()); // Sort wait times to find percentiles
    
    // Calculate 99th percentile tail latency
    int percentileIndex = std::max(0, static_cast<int>(latencies.size() * 0.99) - 1);
    int percentile99th = latencies.empty() ? 0 : latencies[percentileIndex];

    // write job arrival rate, total execution time, average wait time, and average latency to the csv file
    outFile << arrivalRate << "," << totalExecutionTime << "," << averageWaitTime << "," << averageLatency << "," << percentile99th << "\n";

    // Print statistics to the file
    std::cout << "Total execution time for all jobs: " << totalExecutionTime << std::endl;
    std::cout << "Average wait time: " << averageWaitTime << std::endl;
    std::cout << "Average latency: " << averageLatency << std::endl;
    std::cout << "99th percentile tail latency: " << percentile99th << std::endl;
    outFile.flush(); // Ensure statistics are immediately written to the file
    outFile.close(); // Close the file stream
}

int main() {
    int server_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (server_desc == -1) {
        std::cerr << "Could not create socket";
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);

    // Bind the socket to the IP address and port
    if (bind(server_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_desc, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    
    std::cout << "Waiting for incoming connections..." << std::endl;

    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    int client_sock = accept(server_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0) {
        std::cerr << "Accept failed" << std::endl;
        return 1;
    }

    std::thread receiverThread(jobReceiver, client_sock);
    std::thread processorThread(jobProcessor);

    receiverThread.join();
    processorThread.join();

    close(client_sock);
    close(server_desc);

    return 0;
}
