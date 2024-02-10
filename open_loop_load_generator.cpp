#include <iostream>
#include <random>
#include <chrono>
#include <thread>

// Function to generate exponential distribution intervals
double generateExponentialInterval(double lambda) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dis(lambda);

    return dis(gen);
}

// Function to simulate request generation
void generateRequests(double lambda) {
    while (true) {
        // Generate the time interval for the next request
        double interval = generateExponentialInterval(lambda);
        // Convert interval to milliseconds (if necessary)
        auto intervalMs = std::chrono::milliseconds(static_cast<int>(interval * 1000));

        std::cout << "Generating request after " << interval << " seconds." << std::endl;

        // Wait for the time interval before generating the next request
        std::this_thread::sleep_for(intervalMs);

        // Simulate sending a request here
        // sendRequest();
    }
}

int main() {
    double lambda = 0.5; // Adjust lambda as needed. This represents the average number of requests per second.

    generateRequests(lambda);

    return 0;
}
