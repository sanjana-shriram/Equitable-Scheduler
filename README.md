# Equitable-Scheduler

We are studying bias in data center scheduling algorithms. We are generating synthetic load data with various arrival times, job sizes, and demographics and implementing two scheduling algorithms: first come first serve and shortest job first. With this infrastructure in place, we will perform a sensitivity analysis to identify under which distributions of job sizes, demographic splits, and ordering policies we see a scheduling bias against any demographic. As of now, we are developing locally in C++, but we may set up our project on a CloudLab instance.

We generate jobs according to poisson random process.

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

The second sensitivity test we perform varies the order of incoming jobs in our Load Generator. We experiment with multiple ordering policies to evaluate whether they have an effect on the perceived bias experienced by different demographics. In particular, we use uniform random ordering, sequential, batch, and reverse batch ordering. The motivation for the first three ordering techniques stems from the desire to mitigate bias against jobs with demographic A. However, we also experimented with a less intuitive approach in reverse batch which we discuss in detail below. 

Here are the ordering policies we perform the sensitivity analysis with. 
1. Uniform Random Policy
Under the uniform random policy, the Load Generator randomly selects a job to send to the scheduler without considering the demographic of the job.

2. Sequential Policy
Under the sequential policy, the Load Generator sends a job from demographic A to the scheduler, followed by a job from demographic B. This pattern continues, alternating between demographic A and B. For example, if there are three jobs from demographic A and three jobs from demographic B, the final order of sending would be ABABAB.

3. Batch Policy
Under the batch policy, the Load Generator initially sends a set number of jobs from demographic A to the scheduler, then sends a job from demographic B. Subsequently, the Load Generator returns to send the same fixed number of jobs from demographic A to the scheduler. Specifically, this policy examines sending five jobs of demographic A first, followed by one job of demographic B, and then returning to A. For instance, if there are ten jobs from demographic A and two jobs from demographic B, the final order of sending would be AAAAABAAAAAB.

4. Reverse Batch Policy
Under the reverse batch policy, the Load Generator initially sends a set number of jobs from demographic B to the scheduler, then sends a job from demographic A. Subsequently, the Load Generator returns to send the same fixed number of jobs from 
