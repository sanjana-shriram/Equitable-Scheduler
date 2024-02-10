#ifndef JOBTRACE_H
#define JOBTRACE_H

#include <string>

struct JobTrace {
    int arrivalTime; // in milliseconds, represents time between jobs (not absolute arrival time)
    int jobSize;
    char demographic; // 'A' or 'B'
    std::chrono::time_point<std::chrono::system_clock> qRecvTime; // time of arrival in scheduler queue
};

#endif // JOBTRACE_H
