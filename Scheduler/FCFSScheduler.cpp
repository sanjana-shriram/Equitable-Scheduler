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

double computeAverage(int total, int count) {
    return count > 0 ? total / static_cast<double>(count) : 0;
}

int computePercentile(std::vector<int>& latencies, double percentile) {
    if (latencies.empty()) return 0;
    std::sort(latencies.begin(), latencies.end());
    int index = static_cast<int>(std::ceil(percentile * latencies.size())) - 1;
    index = std::max(0, std::min(index, static_cast<int>(latencies.size()) - 1));
    return latencies[index];
}

void jobProcessor() {
    std::unique_lock<std::mutex> lock(queueMutex, std::defer_lock);
    std::ofstream outFile("Results/fcfs_scheduler_results.csv", std::ios_base::app); // Open the output file in append mode

    std::vector<int> waitTimes, waitTimesA, waitTimesB; // Store wait times for all jobs, and separately for jobs A and B
    std::vector<int> latencies, latenciesA, latenciesB; // Store latencies for all jobs, and separately for jobs A and B
    int totalWaitTime = 0, totalWaitTimeA = 0, totalWaitTimeB = 0;
    int totalLatency = 0, totalLatencyA = 0, totalLatencyB = 0;
    int totalJobsProcessed = 0, totalJobsProcessedA = 0, totalJobsProcessedB = 0;
    int totalExecutionTime = 0, totalExecutionTimeA = 0, totalExecutionTimeB = 0; // Execution time tracking
    int arrivalRate;

    while (!finishedReceiving || !jobQueue.empty()) {
        lock.lock();
        queueCondition.wait(lock, [] { return finishedReceiving || !jobQueue.empty(); });

        if (!jobQueue.empty()) {
            JobTrace job = jobQueue.front();
            jobQueue.pop();
            lock.unlock();

            arrivalRate = job.arrivalRate;
            auto startProcessingTime = std::chrono::system_clock::now();
            auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(startProcessingTime - job.qRecvTime).count();
            std::this_thread::sleep_for(std::chrono::milliseconds(job.jobSize)); // Simulate processing
            auto endProcessingTime = std::chrono::system_clock::now();
            auto endToEndLatency = std::chrono::duration_cast<std::chrono::milliseconds>(endProcessingTime - job.qRecvTime).count();
            
            totalWaitTime += waitTime;
            waitTimes.push_back(waitTime);
            latencies.push_back(endToEndLatency);
            totalLatency += endToEndLatency;
            totalJobsProcessed++;
            totalExecutionTime += job.jobSize; // Update overall execution time
            // std::cout << "total exec time: " << totalExecutionTime << " job: " << job.jobID << " demographic " << job.demographic << std::endl;
            
            if (job.demographic == 'A') { // Assuming job has a 'demographic' field
                totalWaitTimeA += waitTime;
                waitTimesA.push_back(waitTime);
                latenciesA.push_back(endToEndLatency);
                totalLatencyA += endToEndLatency;
                totalJobsProcessedA++;
                totalExecutionTimeA += job.jobSize; // Update execution time for A
                // std::cout << "A exec time: " << totalExecutionTimeA << " job: " << job.jobID << " demographic " << job.demographic << std::endl;
            } else {
                totalWaitTimeB += waitTime;
                waitTimesB.push_back(waitTime);
                latenciesB.push_back(endToEndLatency);
                totalLatencyB += endToEndLatency;
                totalJobsProcessedB++;
                totalExecutionTimeB += job.jobSize; // Update execution time for B
                // std::cout << "B exec time: " << totalExecutionTimeB << " job: " << job.jobID << " demographic " << job.demographic << std::endl;
            }
        } else {
            lock.unlock();
        }
    }

    // Compute and output statistics
    outFile << arrivalRate
            << "," << totalExecutionTime
            << "," << totalExecutionTimeA 
            << "," << totalExecutionTimeB
            << "," << computeAverage(totalWaitTime, totalJobsProcessed)
            << "," << computeAverage(totalWaitTimeA, totalJobsProcessedA)
            << "," << computeAverage(totalWaitTimeB, totalJobsProcessedB)
            << "," << computeAverage(totalLatency, totalJobsProcessed)
            << "," << computeAverage(totalLatencyA, totalJobsProcessedA)
            << "," << computeAverage(totalLatencyB, totalJobsProcessedB)
            << "," << computePercentile(latencies, 0.99)
            << "," << computePercentile(latenciesA, 0.99)
            << "," << computePercentile(latenciesB, 0.99)
            << "," << computePercentile(waitTimes, 0.99)
            << "," << computePercentile(waitTimesA, 0.99)
            << "," << computePercentile(waitTimesB, 0.99)
            << std::endl;

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
