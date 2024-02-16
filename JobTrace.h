#ifndef JOBTRACE_H
#define JOBTRACE_H

#include <string>

struct JobTrace {
    int jobID;
    int arrivalTime; // in milliseconds, represents time between jobs (not absolute arrival time)
    int jobSize;
    char demographic; // 'A' or 'B'
    std::chrono::time_point<std::chrono::system_clock> qRecvTime; // time of arrival in scheduler queue

    // Define operator== for JobTrace objects
    bool operator==(const JobTrace& other) const {
        return jobID == other.jobID && arrivalTime == other.arrivalTime && jobSize == other.jobSize &&
               demographic == other.demographic && qRecvTime == other.qRecvTime;
    }

};

#endif // JOBTRACE_H
