# Discrete Time Event Simulator for a Multi-CPU Queuing System
Creator: Heston Montagne

This C++ program simulates an Multi-CPU queuing system representing CPU scheduling based on a First-Come First-Served (FCFS), Shortest Job First (SJF), Shortest Remaining Time First (SRTF), or Highest Response-Ratio Next (HRRN) algorithm. It uses discrete-time events to handle arrivals and departures of processes.

## Table of Contents
 1. [Setup Instructions](#setup-instructions)
 2. [Input Parameters](#input-parameters)
 3. [Output](#output)
 4. [Project Status](#project-status)

## Setup Instructions
### Pre-compiled Executable
Included with the program is a pre-compiled version of the program simulator.exe. To run this file, navigate to the root directory and run the following command. 
~~~
./simulator.exe
~~~
*Note: may not work on linux.*

### Compile and Run
To compile and run the source code (C++), navigate to the root directory and run the following commands.
~~~
make
./simulator.exe
~~~
If on linux, the executable will be of the file type .out, so instead run the following.
~~~
make
./simulator.out
~~~

### Configuration
There are 13 configuration constants that can be updated to change the behavior of the simulation and output. These constants can be found in /config.h. To update one or multiple, edit their value(s) and then recompile the program (see Compile and Run section above).

**PRINT_LIVE_UPDATES** - boolean (default: false) - When true, prints out live updates for every arrival and departure to and from the system, including the below information. *Note: Setting this to true massively slows down the speed of the simulation.*
* Time of the event (in seconds).
* Whether the event is an arrival or departure.
* Whether the CPU is idle.
* What process is moving to the CPU (when it changes).
* Insertions into or pulls from the Ready Queue (if any occur).
* The amount of processes in the Ready Queue after each event.

**DEFAULT_LINE_LENGTH** - integer (default: 50) - Length in characters of lines the simulator uses when formatting and printing most things to the terminal.

**DEFAULT_PADDING** - integer (default: 8) - Number of spaces to leave on the sides of centered text shown in formatted blocks. The value is the sum of the padding for both sides, not the padding for each side (e.g. 8 => 4 spaces on each side). 

**CHOOSE_SCHEDULER** - boolean (default: true) - If true, prompts the user to input which scheduler type the simulation should use. If false, the simulator uses the value of DEFAULT_SCHEDULER.

**CHOOSE_NUM_CPUS** - boolean (default: true) - If true, prompts the user to input how many CPUs the simulation should use. If false, the simulator uses the value of DEFAULT_NUM_CPUS.

**CHOOSE_RQ_SETUP** - boolean (default: true) - If true, prompts the user to input which Ready Queue setup the simulation should use. If false, the simulator uses the value of DEFAULT_RQ_SETUP.

**CHOOSE_END_CONDITION** - boolean (default: true) - If true, prompts the user to input which end condition the simulation should use. Then follow-up prompts the user based on the type of end condition chosen (number of processes or time limit). If false, the simulator uses the value of DEFAULT_END_CONDITION and DEFAULT_N or DEFAULT_TIME_LIMIT (whichever is needed for the end condition determined by DEFAULT_END_CONDITION).

**DEFAULT_SCHEDULER** - integer (default: 0) - Scheduler type the simulator will use if CHOOSE_SCHEDULER is false. 0 for FCFS, 1 for SJF, 2 for SRTF, 3 for HRRN.

**DEFAULT_NUM_CPUS** - integer (default: 1) - Number of CPUs the simulator will use if CHOOSE_NUM_CPUS is false.

**DEFAULT_RQ_SETUP** - integer (default: 2) - Ready Queue setup the simulator will use if CHOOSE_RQ_SETUP is false. 1 for per-CPU Ready Queues, 2 for a single global Ready Queue. 

**DEFAULT_END_CONDITION** - EndCondition (default: PROCESSES_DEPARTED) - End condition that will be used to stop the simulation if CHOOSE_END_CONDITION is false. See /endChecker/EndCondition.h for possible values.

**DEFAULT_N** - integer (default: 10000) - Number of processes arrived/departed the simulator will stop at if the end condition is processes arrived or processes departed.

**DEFAULT_TIME_LIMIT** - integer (default: 100) - Number of in-simulation seconds the simulation will stop after if the end condition is time limit. 

## Input Parameters
With default configuration, the simulator takes 7 arguments (entered in the command line):

**Average Arrival Rate** - float - The average rate at which processes arrive to the system. Measured in processes per second.

**Average Service Time** - float - The average time the CPU takes to service each process. Measured in seconds.

**Scheduler** - 0, 1, or 2 - The scheduler to use. 0 for FCFS, 1 for SJF, 2 for SRTF, 3 for HRRN.

**Ready Queue Scenario** - 1 or 2 - The Ready Queue scenario to use. 1 for per-CPU Ready Queues, 2 for a single global Ready Queue. 

**Number of CPUs** - integer - The number of CPUs to use in the simulation.

**End Condition** - 0, 1, or 2 - The end condition to use to stop the simulation. 0 for number of processes arrived, 1 for number of processes departed, 2 for a time limit (using in-simulation time, not real-time). Based on user input, user will then be prompted for Number of Processes or Time Limit.

* 0 or 1 => **Number of Processes** - integer - The number of processes arrived/departed to stop the simulation after. Only prompted if End Condition was previously prompted and the user input was 0 or 1.

* 2 => **Time Limit** - float - The number of seconds (in-simulation) to stop the simulation after. Only prompted if End Condition was previously prompted and the user input was 2.

## Output
### Metrics
The simulator calculates 4 metrics for the simulated system:
* **Average Turnaround Time** - The average time between a process's arrival to the system and its departure.

* **Total Throughput** - The number of processes done per second.

* **CPU Utilization** - A float (0 to 1) representing the percentage of time that the CPU was busy. If there are multiple CPUs, this metric will be calculated for each CPU. 

* **Average Number of Processes in the Ready Queue(s)** - The average number of processes waiting in the Ready Queue to be scheduled to the CPU. If there are multiple Ready Queues, this metric will be calculated for each Ready Queue.

### Terminal Output
The simulator displays its results to the terminal. It displays markers when stages of the simulator program (initialization, simulation, statistics, and cleanup) have completed. The simulator outputs 4 metrics for the simulated system. These metrics are stated in the Metrics section.

## Project Status
This project is currently *in-progress*. Current development is focusing on adding more scheduler types, adding more dynamic elements to the system, and setting up for a GUI.

### Planned Additions/Changes
* Add more scheduler types: Round Robin

* Introduce a GUI for inputing the parameters of the simulator and viewing the output metrics. 
  * Qt is the current forerunner for which framework to use.
  * A React/React Native wrapper is also an appealing option.

* Introduce ways to unbalance workloads between CPUs
  * Add process types for asymmetric multiprocessing
  * Add processor affinities.

### Possible Additions/Changes
* Add push/pull migration between multiple ready queues for load balancing.
