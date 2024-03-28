import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data into a DataFrame
data = pd.DataFrame({
    "Percentage_A": [0.9, 0.5, 0.1],
    "Avg_Wait_Overall": [1049.37, 548.174, 59.402],
    "Avg_Wait_A": [1061.28, 557.618, 66.702],
    "Avg_Wait_B": [2213, 1134, 135],
    "Avg_Latency_Overall": [1030.36, 543.718, 57.3617],
    "Avg_Latency_A": [1042.99, 556.12, 70.8723],
    "Avg_Latency_B": [2220, 1134, 125],
    "99th_Percentile_Overall": [1209.64, 552.32, 59.6137],
    "99th_Percentile_A": [1215.57, 559.012, 66.2693],
    "99th_Percentile_B": [2185, 1151, 136]
})

fig, axs = plt.subplots(3, 1, figsize=(10, 15))

# Average Wait Time
axs[0].bar(data["Percentage_A"] - 0.03, data["Avg_Wait_Overall"], width=0.02, label='Overall')
axs[0].bar(data["Percentage_A"], data["Avg_Wait_A"], width=0.02, label='A')
axs[0].bar(data["Percentage_A"] + 0.03, data["Avg_Wait_B"], width=0.02, label='B')
axs[0].set_title('Average Wait Time')
axs[0].set_xlabel('Percentage of Jobs that are Demographic A')
axs[0].set_ylabel('Time (ms)')
axs[0].legend()

# Average Latency
axs[1].bar(data["Percentage_A"] - 0.03, data["Avg_Latency_Overall"], width=0.02, label='Overall')
axs[1].bar(data["Percentage_A"], data["Avg_Latency_A"], width=0.02, label='A')
axs[1].bar(data["Percentage_A"] + 0.03, data["Avg_Latency_B"], width=0.02, label='B')
axs[1].set_title('Average Latency')
axs[1].set_xlabel('Percentage of Jobs that are Demographic A')
axs[1].set_ylabel('Time (ms)')
axs[1].legend()

# 99th Percentile Latency
axs[2].bar(data["Percentage_A"] - 0.03, data["99th_Percentile_Overall"], width=0.02, label='Overall')
axs[2].bar(data["Percentage_A"], data["99th_Percentile_A"], width=0.02, label='A')
axs[2].bar(data["Percentage_A"] + 0.03, data["99th_Percentile_B"], width=0.02, label='B')
axs[2].set_title('99th Percentile Latency')
axs[2].set_xlabel('Percentage of Jobs that are Demographic A')
axs[2].set_ylabel('Time (ms)')
axs[2].legend()

plt.tight_layout()
plt.show()
