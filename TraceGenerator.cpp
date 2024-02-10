#include <iostream>
#include <fstream>
#include <random>
#include "JobTrace.h"

double generatePoissonInterval(double lambda) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dis(lambda);
    return dis(gen);
}

int main() {
    std::ofstream outFile("traces.txt");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sizeDist(1, 100); // Adjust range as needed
    std::bernoulli_distribution demoDist(0.5); // 50% chance for 'A' or 'B'
    
    double lambda = 1.0; // Adjust lambda for Poisson process
    const int numTraces = 100; // Number of traces to generate

    for (int i = 0; i < numTraces; ++i) {
        JobTrace trace;
        trace.arrivalTime = static_cast<int>(generatePoissonInterval(lambda) * 1000); // Scale if necessary
        trace.jobSize = sizeDist(gen);
        trace.demographic = demoDist(gen) ? 'A' : 'B';
        outFile << trace.arrivalTime << " " << trace.jobSize << " " << trace.demographic << "\n";
    }

    outFile.close();
    return 0;
}
