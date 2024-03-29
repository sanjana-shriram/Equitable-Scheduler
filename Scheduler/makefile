# Compiler settings - Can be customized.
CXX = g++
# CXXFLAGS = -Wall -std=c++11
CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-c++11-extensions
LDFLAGS = -pthread # Adding linker flags for pthread if using threads

# Build the TraceGenerator, LoadGenerator, and Scheduler programs.
all: TraceGenerator LoadGenerator FCFSScheduler SJFScheduler SJFPreScheduler

TraceGenerator: TraceGenerator.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) -o TraceGenerator TraceGenerator.cpp

LoadGenerator: LoadGenerator.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) -o LoadGenerator LoadGenerator.cpp

FCFSScheduler: FCFSScheduler.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o FCFSScheduler FCFSScheduler.cpp

SJFScheduler: SJFScheduler.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o SJFScheduler SJFScheduler.cpp

SJFPreScheduler: SJFPreScheduler.cpp JobTrace.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o SJFPreScheduler SJFPreScheduler.cpp

# Rule 1: run the TraceGenerator
trace: TraceGenerator
	./TraceGenerator

# Rule 2: run the Scheduler (in one terminal)
scheduler: FCFSScheduler
	./FCFSScheduler

# Rule 3: run the SJFScheduler
sjfscheduler: SJFScheduler
	./SJFScheduler

# Rule 4: run the SJFPreScheduler
sjfprescheduler: SJFPreScheduler
	./SJFPreScheduler

# Rule 5: run the LoadGenerator (with scheduler running in a separate terminal)
loadgen: LoadGenerator
	./LoadGenerator

# Clean the built programs
clean:
	rm -f TraceGenerator LoadGenerator FCFSScheduler SJFScheduler SJFPreScheduler
