/**
 * @file    LoadGenerator2.cpp
 *
 * @brief   Cues all jobs at time = 0
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

    while (inFile >> jobID >> arrivalTime >> jobSize >> demographic) {
        JobTrace job{jobID, arrivalTime, jobSize, demographic};
        // Don't wait for arrival time to elapse before sending a job to scheduler
        send(socket_desc, &job, sizeof(job), 0);
    }

    inFile.close();
    close(socket_desc);
    return 0;
}
