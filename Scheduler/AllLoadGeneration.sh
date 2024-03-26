#!/bin/bash

# Compile the program
g++ -o LoadGenerator LoadGenerator.cpp

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Loop through a range of arrival rates
for ((rate = 1; rate <= 10; rate+=4))
do
   # Execute the program with the current arrival rate
   ./LoadGenerator $rate
done