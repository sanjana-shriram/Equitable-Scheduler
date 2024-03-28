#!/bin/bash

# Compile the program (select the scheduler you want to run)
g++ -o FCFSScheduler FCFSScheduler.cpp
g++ -o LoadGenerator LoadGenerator.cpp

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Loop through a range of arrival rates
for ((rate = 20; rate <= 240; rate += 5))
do
   echo "Starting FCFS Scheduler for arrival rate: $rate"
    # Start FCFS Scheduler in the background
    ./FCFSScheduler &
    SCHEDULER_PID=$!

    sleep 1

    echo "Running Load Generator with arrival rate: $rate"
    # Execute Load Generator with the current arrival rate
    ./LoadGenerator $rate

    # After LoadGenerator finishes, send a shutdown signal to the scheduler.
    # This assumes LoadGenerator sends a recognizable shutdown signal as its last action.
    wait $SCHEDULER_PID

    echo "FCFS Scheduler completed for arrival rate: $rate."
    # Optional: Wait a bit before starting the next iteration
    sleep 1
done