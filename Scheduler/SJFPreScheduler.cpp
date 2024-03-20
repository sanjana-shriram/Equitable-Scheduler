/**
 * @file    SJFPreScheduler.cpp
 *
 * @brief   Shortest Job First Scheduling with Pre-emption
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
#include "JobTrace.h" // Your JobTrace structure needs to be defined

std::deque<JobTrace> jobQueue;
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
            std::lock_guard<std::mutex> lock(queueMutex);
            job.qRecvTime = std::chrono::system_clock::now();
            jobQueue.push_back(job);
            // Sort the jobQueue based on job size (or any other criterion)
            std::sort(jobQueue.begin(), jobQueue.end(), [](const JobTrace& a, const JobTrace& b) {
                return a.jobSize < b.jobSize; // Sort by job size in ascending order
            });
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
    std::ofstream outFile("results_1_sjf_pre_scheduler.txt"); // Open the output file

    while (!finishedReceiving || !jobQueue.empty()) {
        lock.lock();
        queueCondition.wait(lock, [] { return finishedReceiving || !jobQueue.empty(); });

        if (!jobQueue.empty()) {
            JobTrace job = jobQueue.front();
            jobQueue.pop_front();
            lock.unlock();

            auto startProcessingTime = std::chrono::system_clock::now();
            auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(startProcessingTime - job.qRecvTime).count();

            // Update total wait time
            totalWaitTime += waitTime;

            // Check if there is a shorter job in the queue
            bool preempted = false;
            lock.lock();
            for (const auto& otherJob : jobQueue) {
                if (otherJob.jobSize < job.jobSize) {
                    // Preempt the current job and process the shorter one
                    jobQueue.push_front(job); // Re-insert preempted job at the front
                    job = otherJob; // Switch to the shorter job
                    jobQueue.erase(std::find(jobQueue.begin(), jobQueue.end(), otherJob)); // Remove the shorter job from the queue
                    preempted = true;
                    break;
                }
            }
            lock.unlock();

            // Process the job if not preempted
            if (!preempted) {
                std::this_thread::sleep_for(std::chrono::milliseconds(job.jobSize)); // Simulate processing
                auto endProcessingTime = std::chrono::system_clock::now();
                auto endToEndLatency = std::chrono::duration_cast<std::chrono::milliseconds>(endProcessingTime - job.qRecvTime).count();

                // Update total latency
                totalLatency += endToEndLatency;

                // Update total execution time
                totalExecutionTime += job.jobSize;

                outFile << "Processed job " << job.jobID 
                        << " of size " << job.jobSize
                        << " with wait time " << waitTime << " milliseconds"
                        << ", end-to-end latency " << endToEndLatency << " milliseconds." << "\n";
                outFile.flush();

                // Increment total jobs processed
                totalJobsProcessed++;
            }
        } else {
            lock.unlock();
        }
    }

    // Calculate and print statistics
    double averageWaitTime = static_cast<double>(totalWaitTime) / totalJobsProcessed;
    double averageLatency = static_cast<double>(totalLatency) / totalJobsProcessed;

    outFile << "\nStatistics:" << "\n";
    outFile << "Total execution time for all jobs: " << totalExecutionTime << " milliseconds" << "\n";
    outFile << "Average wait time: " << averageWaitTime << " milliseconds" << "\n";
    outFile << "Average latency: " << averageLatency << " milliseconds" << "\n";
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