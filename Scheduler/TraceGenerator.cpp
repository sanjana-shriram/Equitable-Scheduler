#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib> // For std::atof
#include "JobTrace.h"
#include <sstream> // For filename

double generatePoissonInterval(double lambda) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dis(lambda);
    return dis(gen);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <arrival rate>" << std::endl;
        return 1;
    }

    double lambda = std::atof(argv[1]); // Convert command-line argument to double

    // Construct the output filename
    std::ostringstream filename;
    filename << "Traces/traces_" << lambda << ".txt";
    
    std::ofstream outFile(filename.str());
    if (!outFile) {
        std::cerr << "Failed to open file: " << filename.str() << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sizeDist(1, 100); // Adjust range as needed
    std::bernoulli_distribution demoDist(0.5); // 50% chance for 'A' or 'B'
    
    const int numTraces = 100; // Number of traces to generate

    for (int i = 0; i < numTraces; ++i) {
        JobTrace trace;
        trace.jobID = i+1;
        trace.arrivalTime = static_cast<int>(generatePoissonInterval(lambda) * 1000); // Scale if necessary
        trace.arrivalRate = static_cast<int>(lambda);
        trace.demographic = demoDist(gen) ? 'A' : 'B';

        // if demographic 'A', set jobSize to 2x jobSize
        trace.jobSize = sizeDist(gen); 
        if (trace.demographic == 'A') {
            trace.jobSize = trace.jobSize * 2;
        }
        outFile << trace.jobID << " " << trace.arrivalTime << " " << trace.jobSize << " " << trace.arrivalRate << " " << trace.demographic << "\n";
    }

    outFile.close();
    return 0;
}