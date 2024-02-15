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

# Rule 1: run the TraceGenerator
trace: TraceGenerator
	./TraceGenerator

# Rule 2: run the Scheduler (in one terminal)
scheduler: FCFSScheduler
	./FCFSScheduler

# Rule 3: run the LoadGenerator (with scheduler running in a separate terminal)
test: LoadGenerator
	./LoadGenerator

# Clean the built programs
clean:
	rm -f TraceGenerator LoadGenerator FCFSScheduler
