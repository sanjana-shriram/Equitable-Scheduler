#ifndef JOBTRACE_H
#define JOBTRACE_H

#include <string>


struct JobTrace {
    int jobID;
    double arrivalTime; // in milliseconds, represents time between jobs (not absolute arrival time)
    int jobSize;
    int arrivalRate;
    char demographic; // 'A' or 'B'
    std::chrono::time_point<std::chrono::system_clock> qRecvTime; // time of arrival in scheduler queue


    // Constructor with parameters
    // JobTrace(int id, int time, int size, int rate, char demo, const std::chrono::time_point<std::chrono::system_clock>& recvTime)
    //     : jobID(id), arrivalTime(time), jobSize(size), arrivalRate(rate), demographic(demo), qRecvTime(recvTime) {}

    // Define operator== for JobTrace objects
    bool operator==(const JobTrace& other) const {
        return  jobID == other.jobID && 
                arrivalTime == other.arrivalTime && 
                jobSize == other.jobSize && 
                arrivalRate == other.arrivalRate &&
                demographic == other.demographic && 
                qRecvTime == other.qRecvTime;
        }
};

#endif // JOBTRACE_H
