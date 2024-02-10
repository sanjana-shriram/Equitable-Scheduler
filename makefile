# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -pthread # Adding linker flags for pthread if using threads

# Build the TraceGenerator, LoadGenerator, and Scheduler programs.
all: TraceGenerator LoadGenerator Scheduler

TraceGenerator: TraceGenerator.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) -o TraceGenerator TraceGenerator.cpp

LoadGenerator: LoadGenerator.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) -o LoadGenerator LoadGenerator.cpp

FCFSScheduler: FCFSScheduler.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o FCFSScheduler FCFSScheduler.cpp

# Rule to run TraceGenerator to generate the trace file, then run LoadGenerator.
run: TraceGenerator LoadGenerator
	./TraceGenerator
	./LoadGenerator

# Clean the built programs
clean:
	rm -f TraceGenerator LoadGenerator FCFSScheduler
