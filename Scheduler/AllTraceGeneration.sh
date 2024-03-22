#!/bin/bash

# Compile the program
g++ -o TraceGenerator TraceGenerator.cpp

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Loop through a range of arrival rates
for ((rate = 5; rate <= 10; rate++)) # Example: rates from 5 to 10 with step of 1
do
   # Execute the program with the current arrival rate
   ./TraceGenerator $rate
done
