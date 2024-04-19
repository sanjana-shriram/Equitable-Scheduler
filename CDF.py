import numpy as np
import matplotlib.pyplot as plt

# Latency data for demographic A and B
latencies_A = [2,7,10,13,16,20,21,23,31,50,127]
latencies_B = [1,3,5,6,8,9,11,12,15,19,71]

# Wait time data for demographic A and B
wait_times_A = [0,0,0,0,0,1,5,10,13,25,107]
wait_times_B = [0,0,0,0,0,0,2,4,7,11,58]

# Function to calculate CDF
def calculate_cdf(data):
    data_sorted = np.sort(data)
    cdf_values = np.arange(1, len(data_sorted) + 1) / len(data_sorted)
    return data_sorted, cdf_values

# Calculate CDF for latencies
latencies_A_sorted, cdf_latencies_A = calculate_cdf(latencies_A)
latencies_B_sorted, cdf_latencies_B = calculate_cdf(latencies_B)

# Calculate CDF for wait times
wait_times_A_sorted, cdf_wait_times_A = calculate_cdf(wait_times_A)
wait_times_B_sorted, cdf_wait_times_B = calculate_cdf(wait_times_B)

# Plot CDF for latencies
plt.figure(figsize=(5, 5))
plt.plot(latencies_A_sorted, cdf_latencies_A, marker='o', linestyle='-', label='Demographic A')
plt.plot(latencies_B_sorted, cdf_latencies_B, marker='o', linestyle='-', label='Demographic B')
plt.title('Reverse Batch 0.1 Latency CDF')
plt.xlabel('Latency (ms)')
plt.ylabel('CDF')
plt.xlim(0, 800)
plt.legend()
plt.grid(True)
plt.show()

# Plot CDF for wait times
plt.figure(figsize=(5, 5))
plt.plot(wait_times_A_sorted, cdf_wait_times_A, marker='o', linestyle='-', label='Demographic A')
plt.plot(wait_times_B_sorted, cdf_wait_times_B, marker='o', linestyle='-', label='Demographic B')
plt.title('Reverse Batch 0.1 Wait Time CDF')
plt.xlabel('Wait Time (ms)')
plt.ylabel('CDF')
plt.xlim(0, 800)
plt.legend()
plt.grid(True)
plt.show()
