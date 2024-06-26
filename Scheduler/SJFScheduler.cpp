/**
 * @file    SJFScheduler.cpp
 *
 * @brief   Shortest Job First Scheduling without Pre-emption
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
            // std::cout << "Received Job:" << std::endl;
            // std::cout << "Job ID: " << job.jobID << std::endl;
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
    std::ofstream outFile("Results/sjf_scheduler_results.csv", std::ios_base::app);
    
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
            jobQueue.pop_front();
            lock.unlock();

            // std::cout << "Processing Job:" << std::endl;
            // std::cout << "Job ID: " << job.jobID << std::endl;
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
            totalExecutionTime += job.jobSize;

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

    // Calculate average wait time and average latency
    // double averageWaitTime = totalWaitTime / static_cast<double>(totalJobsProcessed);
    // double averageLatency = totalLatency / static_cast<double>(totalJobsProcessed);

   // Print statistics to the file
    outFile << arrivalRate
            // << "," << totalExecutionTime
            // << "," << totalExecutionTimeA 
            // << "," << totalExecutionTimeB
            // << "," << computeAverage(totalWaitTime, totalJobsProcessed)
            // << "," << computeAverage(totalWaitTimeA, totalJobsProcessedA)
            // << "," << computeAverage(totalWaitTimeB, totalJobsProcessedB)
            // << "," << computeAverage(totalLatency, totalJobsProcessed)
            // << "," << computeAverage(totalLatencyA, totalJobsProcessedA)
            // << "," << computeAverage(totalLatencyB, totalJobsProcessedB)
            // << "," << computePercentile(latencies, 0.99)
            << "," << computePercentile(latenciesA, 0)
            << "," << computePercentile(latenciesA, 0.1)
            << "," << computePercentile(latenciesA, 0.2)
            << "," << computePercentile(latenciesA, 0.3)
            << "," << computePercentile(latenciesA, 0.4)
            << "," << computePercentile(latenciesA, 0.5)
            << "," << computePercentile(latenciesA, 0.6)
            << "," << computePercentile(latenciesA, 0.7)
            << "," << computePercentile(latenciesA, 0.8)
            << "," << computePercentile(latenciesA, 0.9)
            << "," << computePercentile(latenciesA, 1)
            << "," << computePercentile(latenciesB, 0)
            << "," << computePercentile(latenciesB, 0.1)
            << "," << computePercentile(latenciesB, 0.2)
            << "," << computePercentile(latenciesB, 0.3)
            << "," << computePercentile(latenciesB, 0.4)
            << "," << computePercentile(latenciesB, 0.5)
            << "," << computePercentile(latenciesB, 0.6)
            << "," << computePercentile(latenciesB, 0.7)
            << "," << computePercentile(latenciesB, 0.8)
            << "," << computePercentile(latenciesB, 0.9)
            << "," << computePercentile(latenciesB, 1)
            // << "," << computePercentile(waitTimes, 0.99)
            << "," << computePercentile(waitTimesA, 0)
            << "," << computePercentile(waitTimesA, 0.1)
            << "," << computePercentile(waitTimesA, 0.2)
            << "," << computePercentile(waitTimesA, 0.3)
            << "," << computePercentile(waitTimesA, 0.4)
            << "," << computePercentile(waitTimesA, 0.5)
            << "," << computePercentile(waitTimesA, 0.6)
            << "," << computePercentile(waitTimesA, 0.7)
            << "," << computePercentile(waitTimesA, 0.8)
            << "," << computePercentile(waitTimesA, 0.9)
            << "," << computePercentile(waitTimesA, 1)
            << "," << computePercentile(waitTimesB, 0)
            << "," << computePercentile(waitTimesB, 0.1)
            << "," << computePercentile(waitTimesB, 0.2)
            << "," << computePercentile(waitTimesB, 0.3)
            << "," << computePercentile(waitTimesB, 0.4)
            << "," << computePercentile(waitTimesB, 0.5)
            << "," << computePercentile(waitTimesB, 0.6)
            << "," << computePercentile(waitTimesB, 0.7)
            << "," << computePercentile(waitTimesB, 0.8)
            << "," << computePercentile(waitTimesB, 0.9)
            << "," << computePercentile(waitTimesB, 1)
            << std::endl;

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



// /**
//  * @file    SJFScheduler.cpp
//  *
//  * @brief   Shortest Job First Scheduling without Pre-emption
//  */

// #include <iostream>
// #include <fstream>
// #include <queue>
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include "JobTrace.h" // Your JobTrace structure needs to be defined

// std::deque<JobTrace> jobQueue;
// std::mutex queueMutex;
// std::condition_variable queueCondition;
// bool finishedReceiving = false;
// int totalExecutionTime = 0;
// int totalWaitTime = 0;
// int totalLatency = 0;
// int totalJobsProcessed = 0;

// void jobReceiver(int client_sock) {
//     // new
//     std::vector<long long> endToEndLatencies; // Store end-to-end latencies for percentile calculation
    
//     while (true) {
//         JobTrace job;
//         ssize_t read_size = recv(client_sock, &job, sizeof(job), 0);
//         if (read_size > 0) {
//             std::lock_guard<std::mutex> lock(queueMutex);
//             job.qRecvTime = std::chrono::system_clock::now();
//             jobQueue.push_back(job);
//             // Sort the jobQueue based on job size (or any other criterion)
//             std::sort(jobQueue.begin(), jobQueue.end(), [](const JobTrace& a, const JobTrace& b) {
//                 return a.jobSize < b.jobSize; // Sort by job size in ascending order
//             });
//             queueCondition.notify_one(); // Notify the processor thread
//         } else {
//             if (read_size == 0) {
//                 std::cout << "Client disconnected." << std::endl;
//             } else if (read_size == -1) {
//                 std::cerr << "Recv failed." << std::endl;
//             }
//             break;
//         }
//     }
//     finishedReceiving = true;
//     queueCondition.notify_all(); // Ensure the processor thread can exit if waiting
// }


// // new
// // Define a custom comparator for the copy of priority_queue
// struct greater_than {
//     bool operator()(const long long& a, const long long& b) const {
//         return a > b;
//     }
// };


// // new
// #include <queue>
// #include <vector>

// double calculateAverage(const std::priority_queue<long long, std::vector<long long>, std::greater<long long>>& maxLatencies) {
//     // Calculate the sum of all elements
//     long long sum = 0;
//     size_t count = 0;

//     // Create a copy of the input priority queue since we'll modify it
//     std::priority_queue<long long, std::vector<long long>, std::greater<long long>> pq = maxLatencies;

//     while (!pq.empty()) {
//         sum += pq.top();
//         pq.pop();
//         count++;
//     }

//     // Calculate the average
//     double average = (count == 0) ? 0 : static_cast<double>(sum) / count;

//     return average;
// }




// void jobProcessor() {
//     std::unique_lock<std::mutex> lock(queueMutex, std::defer_lock);
//     int arrivalRate = 0;
//     // new
//     std::priority_queue<long long, std::vector<long long>, std::greater<long long>> maxLatencies; // Priority queue to store the top 50 maximum latencies

//     while (!finishedReceiving || !jobQueue.empty()) {
//         lock.lock();
//         queueCondition.wait(lock, [] { return finishedReceiving || !jobQueue.empty(); });

//         if (!jobQueue.empty()) {
//             JobTrace job = jobQueue.front();
//             jobQueue.pop_front();
//             lock.unlock();

//             arrivalRate = job.arrivalRate;

//             auto startProcessingTime = std::chrono::system_clock::now();
//             auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(startProcessingTime - job.qRecvTime).count();

//             // Update total wait time
//             totalWaitTime += waitTime;

//             std::this_thread::sleep_for(std::chrono::milliseconds(job.jobSize)); // Simulate processing

//             auto endProcessingTime = std::chrono::system_clock::now();
//             auto endToEndLatency = std::chrono::duration_cast<std::chrono::milliseconds>(endProcessingTime - job.qRecvTime).count();

//             // Update total latency
//             totalLatency += endToEndLatency;

//             // Update total execution time
//             totalExecutionTime += job.jobSize;

//             // std::ofstream outFile("results_1_sjf_scheduler.txt"); // Open the output file
//             // outFile << "Processed job " << job.jobID 
//             //         << " of size " << job.jobSize
//             //         << " with wait time " << waitTime << " milliseconds"
//             //         << ", end-to-end latency " << endToEndLatency << " milliseconds." << "\n";
//             // outFile.flush();

//             // Store the latency in the priority queue
//             maxLatencies.push(endToEndLatency);
//             if (maxLatencies.size() > 50) {
//                 maxLatencies.pop(); // Keep only top 50 maximum latencies
//             }

//             // Increment total jobs processed
//             totalJobsProcessed++;
//         } else {
//             lock.unlock();
//         }
//     }

//     // Calculate average wait time and average latency
//     double averageWaitTime = totalWaitTime / static_cast<double>(totalJobsProcessed);
//     double averageLatency = totalLatency / static_cast<double>(totalJobsProcessed);
//     double averageTailLatency = calculateAverage(maxLatencies);

//     std::ofstream outFile("results_sjf_scheduler.csv", std::ios::app);
    
//    // Print statistics to the file
//     outFile << arrivalRate << ", " << totalExecutionTime << ", " << averageWaitTime << ", " << averageTailLatency  << ", " << averageLatency  << "\n";
//     outFile.flush(); // Ensure statistics are immediately written to the file
//     outFile.close(); // Close the file stream
// }


// int main() {
    
//     int server_desc = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_desc == -1) {
//         std::cerr << "Could not create socket";
//         return 1;
//     }

//     struct sockaddr_in server;
//     server.sin_family = AF_INET;
//     server.sin_addr.s_addr = inet_addr("127.0.0.1");
//     server.sin_port = htons(8888);

//     // Bind the socket to the IP address and port
//     if (bind(server_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
//         std::cerr << "Bind failed" << std::endl;
//         return 1;
//     }

//     // Listen for incoming connections
//     if (listen(server_desc, 3) < 0) {
//         std::cerr << "Listen failed" << std::endl;
//         return 1;
//     }
    
//     std::cout << "Waiting for incoming connections..." << std::endl;
//     std::vector<std::thread> threads;
//     std::deque<int> clientDeque;
//     while(true) {
//         struct sockaddr_in client;
//         int c = sizeof(struct sockaddr_in);
//         int client_sock = accept(server_desc, (struct sockaddr*)&client, (socklen_t*)&c);
//         if (client_sock < 0) {
//             std::cerr << "Accept failed" << std::endl;
//             return 1;
//         }

//         threads.push_back(std::thread(jobReceiver, client_sock));
//         threads.push_back(std::thread(jobProcessor));
//         clientDeque.emplace_back(client_sock);
//         // receiverThread.join();
//         // processorThread.join();

//         // close(client_sock);
//     }

//     for (auto& thread : threads) {
//         thread.join();
//     }
//     for (auto& client_sock : clientDeque) {
//         close(client_sock);
//     }
 
//     close(server_desc); 
//     return 0;
// }
