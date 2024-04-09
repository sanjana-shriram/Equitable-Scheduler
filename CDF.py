import csv
import matplotlib.pyplot as plt
import numpy as np

# Function to read data from CSV file
def read_csv(filename):
    latenciesA = []
    latenciesB = []

    with open(filename, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            latenciesA.extend(map(float, row[5:15]))  # Change column indices according to your data
            latenciesB.extend(map(float, row[15:25]))  # Change column indices according to your data

    return latenciesA, latenciesB

# Function to plot CDF
def plot_cdf(latenciesA, latenciesB):
    plt.figure(figsize=(10, 5))

    # Calculate CDF values
    sorted_data_A = np.sort(latenciesA)
    sorted_data_B = np.sort(latenciesB)
    y_values_A = np.arange(0, len(sorted_data_A)) / len(sorted_data_A)
    y_values_B = np.arange(0, len(sorted_data_B)) / len(sorted_data_B)

    # Plot CDF for Demographic A
    plt.plot(sorted_data_A, y_values_A, marker='o', linestyle='-', label='Demographic A')

    # Plot CDF for Demographic B
    plt.plot(sorted_data_B, y_values_B, marker='o', linestyle='-', label='Demographic B')

    plt.title('Cumulative Distribution Function (CDF) of Latencies')
    plt.xlabel('Latency')
    plt.ylabel('CDF')
    plt.legend()
    plt.grid(True)

    plt.show()

# Main function
if __name__ == "__main__":
    filename = 'Scheduler/Results/sjf_'  # Change this to your CSV file path
    latenciesA, latenciesB = read_csv(filename)
    plot_cdf(latenciesA, latenciesB)
