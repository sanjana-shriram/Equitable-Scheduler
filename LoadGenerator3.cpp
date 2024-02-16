/**
 * @file    LoadGenerator3.cpp
 *
 * @brief   Cues jobs at random intervals 
 */


#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "JobTrace.h"
#include <unistd.h>
#include <random>

// Function to generate a Poisson-distributed random interval
double generatePoissonInterval(double lambda) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dis(lambda);
    return dis(gen);
}

int main() {
    std::ifstream inFile("traces.txt");
    int jobID, arrivalTime, jobSize;
    char demographic;

    // Create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        std::cerr << "Could not create socket";
        return 1;
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Connect to scheduler
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed";
        return 1;
    }

    /*
    Low Lambda = slower rate, distribution is more skewed towards lower values. 
                Jobs arrive relatively infrequently.
    High Lambda = faster rate, distribution is spread out towards higher values. 
                Jobs arrive more frequently.
    
    Lambda = 20 - leads to a few preemptions here and there
    Labmda = 50 - leads to a few preemptions as well
    */

    double lambda = 50.0; // Adjust lambda for Poisson process

    while (inFile >> jobID >> arrivalTime >> jobSize >> demographic) {
        JobTrace job{jobID, arrivalTime, jobSize, demographic};
        
        // Generate a random interval for job arrival
        double interval = generatePoissonInterval(lambda);
        int sleepTime = static_cast<int>(interval * 1000); // Convert to milliseconds

        // Sleep for the random interval
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // Send the job to the scheduler
        send(socket_desc, &job, sizeof(job), 0);
    }
    inFile.close();
    close(socket_desc);
    return 0;
}
