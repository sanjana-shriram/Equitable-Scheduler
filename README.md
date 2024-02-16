# Equitable-Scheduler

We are studying bias in data center scheduling algorithms. We are generating synthetic load data with various arrival times, job sizes, and demographics and implementing a few scheduling algorithms including first come first serve, preemptive shortest job first, and non-preemptive shortest job first. With this infrastructure in place, we will perform a sensitivity analysis to identify under which distributions of arrival times, job sizes, and demographics we see a scheduling bias against any demographic. As of now, we are developing locally in C++, but we may set up our project on a CloudLab instance.

This project is still in development, however here is our current progress.

We generate 2 synthetic loads:
1. Load 1 - cues jobs according to poisson random process
2. Load 2 - cues all jobs at time = 0

We utilize 3 different scheduling algorithms with the following characteristics:
1. First Come First Serve
- The process that requests the CPU is serviced first
- Uses a FIFO queue
- Inefficient, high wait times
- Fair algorithm
2. Shortest Job First
- The shortest job in the queue is serviced first
- Has the shortest wait time among all scheduling algorithms
- Greedy
- Can cause starvation
3. Shortest Job First with Preemption
- A process executes first when it has the shortest burst time
- If a process with a shorter burst time is queued, the current process receives is preempted/removed from being executed
