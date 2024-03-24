import csv
import matplotlib.pyplot as plt

# Function to read data from CSV file
def read_csv(filename):
    arrival_rate = []
    total_execution_time = []
    average_wait_time = []
    average_latency = []

    with open(filename, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            if row[0][0] == '0':
                continue  # Ignore lines starting with '0'
            arrival_rate.append(int(row[0]))
            total_execution_time.append(float(row[1]))
            average_wait_time.append(float(row[2]))
            average_latency.append(float(row[3]))

    return arrival_rate, total_execution_time, average_wait_time, average_latency

# Function to plot graphs
def plot_graphs(arrival_rate, total_execution_time, average_wait_time, average_latency):
    plt.figure(figsize=(10, 8))

    # Plot Total Execution Time vs Arrival Rate
    plt.subplot(3, 1, 1)
    plt.plot(arrival_rate, total_execution_time, marker='o', linestyle='-')
    plt.title('Total Execution Time vs Arrival Rate')
    plt.xlabel('Arrival Rate')
    plt.ylabel('Total Execution Time')

    # Plot Average Wait Time vs Arrival Rate
    plt.subplot(3, 1, 2)
    plt.plot(arrival_rate, average_wait_time, marker='o', linestyle='-')
    plt.title('Average Wait Time vs Arrival Rate')
    plt.xlabel('Arrival Rate')
    plt.ylabel('Average Wait Time')

    # Plot Average Latency vs Arrival Rate
    plt.subplot(3, 1, 3)
    plt.plot(arrival_rate, average_latency, marker='o', linestyle='-')
    plt.title('Average Latency vs Arrival Rate')
    plt.xlabel('Arrival Rate')
    plt.ylabel('Average Latency')

    plt.tight_layout()
    plt.show()

# Main function
if __name__ == "__main__":
    filename = 'Scheduler/results_sjf_scheduler.csv'  # Change this to your CSV file path
    arrival_rate, total_execution_time, average_wait_time, average_latency = read_csv(filename)
    plot_graphs(arrival_rate, total_execution_time, average_wait_time, average_latency)
