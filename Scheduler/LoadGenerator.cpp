#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "JobTrace.h"
#include <unistd.h>
#include <sstream> // For constructing the file name

int main(int argc, char* argv[]) {
    std::cout << "entering main" << std::endl;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <arrival rate>" << std::endl;
        return 1;
    }

    std::cout << "constructing input file name" << std::endl;
    // Construct the input file name based on the arrival rate
    std::ostringstream filename;
    // filename << "Traces/traces_" << argv[1] << ".txt";
    filename << "Traces/traces_80_90A_10B.txt";
    std::ifstream inFile(filename.str());
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << filename.str() << std::endl;
        return 1;
    }

    int jobID, arrivalTime, jobSize, arrivalRate;
    char demographic;
    // std::chrono::time_point<std::chrono::system_clock> qRecvTime = std::chrono::system_clock::now();
    // std::chrono::system_clock::now();

    // Create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        std::cerr << "Could not create socket";
        return 1;
    }

    std::cout << "created socket" << std::endl;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Connect to scheduler
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed";
        close(socket_desc);
        return 1;
    }

    // store A jobs and B jobs separately
    std::vector<JobTrace> aJobs, bJobs;

    while (inFile >> jobID >> arrivalTime >> jobSize >> arrivalRate >> demographic) {
        // JobTrace job{jobID, arrivalTime, jobSize, arrivalRate, demographic, std::chrono::system_clock::now()};
        JobTrace job;
        job.jobID = jobID;
        job.arrivalTime = arrivalTime;
        job.jobSize = jobSize;
        job.arrivalRate = arrivalRate;
        job.demographic = demographic;
        job.qRecvTime = std::chrono::system_clock::now();

        if (demographic == 'A') {
            aJobs.push_back(job);
        } else if (demographic == 'B') {
            bJobs.push_back(job);
        }
        
        // No queueing policy 
        // std::this_thread::sleep_for(std::chrono::milliseconds(arrivalTime));
        // send(socket_desc, &job, sizeof(job), 0);
    }

    

    // get length of aJobs
    // int aJobsLength = aJobs.size();

    // Batch Policy (switched A and B)
    int aCount = 0;
    int bCount = 0;
    while (!aJobs.empty() && !bJobs.empty()) { 
        // Code for reverse batch is implemented currently 
        if (aCount == 1 || bCount != 5) { // change number 
            // send B job
            JobTrace job = bJobs.front();
            // wait before sending
            std::this_thread::sleep_for(std::chrono::milliseconds(job.arrivalTime));
            send(socket_desc, &job, sizeof(job), 0);
            bJobs.erase(bJobs.begin());
            bCount++;
            aCount = 0;
           
        } else {
            // send B job
            JobTrace job = aJobs.front();
            // wait before sending
            std::this_thread::sleep_for(std::chrono::milliseconds(job.arrivalTime));
            send(socket_desc, &job, sizeof(job), 0);
            aJobs.erase(aJobs.begin());
            aCount = 1;
            bCount = 0;
        }
    }
    
    // Finish running the B jobs with no policy
    while (!bJobs.empty()) {
        JobTrace job = bJobs.front();
        // wait before sending
        std::this_thread::sleep_for(std::chrono::milliseconds(job.arrivalTime));
        send(socket_desc, &job, sizeof(job), 0);
        bJobs.erase(bJobs.begin());
    }


    // Finish running the A jobs with no policy
    while (!aJobs.empty()) {
        JobTrace job = aJobs.front();
        // wait before sending
        std::this_thread::sleep_for(std::chrono::milliseconds(job.arrivalTime));
        send(socket_desc, &job, sizeof(job), 0);
        aJobs.erase(aJobs.begin());
    }

    inFile.close();
    close(socket_desc);
    return 0;
}
