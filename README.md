# Discrete Time Event Simulator for a Multi-CPU Queuing System
Creator: Heston Montagne

This C++ program simulates an Multi-CPU queuing system representing CPU scheduling based on a First-Come First-Served (FCFS) algorithm or a Shortest Job First (SJF) algorithm. 

## Table of Contents
 1. [Setup Instructions](#setup-instructions)
 2. [Input Parameters](#input-parameters)
 3. [Output](#output)

## Setup Instructions
### Pre-compiled Executable
Included with the program is a pre-compiled version of the program simulator.exe. To run this file, navigate to the HM_Homework_4 directory and run the following command. 
~~~
./simulator.exe
~~~
*Note: may not work on linux.*

### Compile and Run
To compile and run the source code (C++), navigate to the HM_Homework_5 directory and run the following commands.
~~~
g++ simulator.cpp
./a.exe
~~~
If on linux, the executable will be of the file type .out, so instead run the following.
~~~
g++ simulator.cpp
./a.out
~~~

### Configuration
There are 8 configuration constants that can be updated to change the behavior of the simulation and output. To update one or multiple, edit their value(s) and then recompile the program (see Compile and Run section above).

**PRINT_LIVE_UPDATES** - boolean (default: false) - When true, prints out live updates for every arrival and departure to and from the system, including the below information. *Note: Setting this to true massively slows down the speed of the simulation.*
* Time of the event (in seconds).
* Whether the event is an arrival or departure.
* Whether the CPU is idle.
* What process is moving to the CPU (when it changes).
* Insertions into or pulls from the Ready Queue (if any occur).
* The amount of processes in the Ready Queue after each event.

**N** - integer (default: 10000) - Used to determine when to stop the simulation. The simulation halts after the Nth process completes and departs from the system. Any processes still in the system when the simulation stops are not resolved.

**CHOOSESCHEDULER** - boolean (default: false) - If true, prompts the user to input which scheduler type the simulation should use. If false, the simulator uses the value of DEFAULTSCHEDULER.

**DEFAULTSCHEDULER** - integer (default: 0) - Scheduler type the simulator will use if CHOOSESCHEDULER is false. 0 for FCFS, 1 for SJF.

**CHOOSENUMCPUS** - boolean (default: true) - If true, prompts the user to input how many CPUs the simulation should use. If false, the simulator uses the value of DEFAULTNUMCPUS.

**DEFAULTNUMCPUS** - integer (default: 1) - Number of CPUs the simulator will use if CHOOSENUMCPUS is false.

**CHOOSERQSETUP** - boolean (default: true) - If true, prompts the user to input which Ready Queue setup the simulation should use. If false, the simulator uses the value of DEFAULTRQSETUP.

**DEFAULTRQSETUP** - integer (default: 2) - Ready Queue setup the simulator will use if CHOOSERQSETUP is false. 1 for per-CPU Ready Queues, 2 for a single global Ready Queue. 

## Input Parameters
With default configuration, the simulator takes 4 arguments (entered in the command line):

**Average Arrival Rate** - float - The average rate at which processes arrive to the system. Measured in processes per second.

**Average Service Time** - float - The average time the CPU takes to service each process. Measured in seconds.

**Ready Queue Scenario** - 1 or 2 - The Ready Queue scenario to use. 1 for per-CPU Ready Queues, 2 for a single global Ready Queue. 

**Number of CPUs** - integer - The number of CPUs to use in the simulation.

## Output
### Metrics
The simulator calculates 4 metrics for the simulated system:
* Average Turnaround Time - The average time between a process's arrival to the system and its departure.
* Total Throughput - The number of processes done per second.
* CPU Utilization - A float (0 to 1) representing the percentage of time that the CPU was busy. If there are multiple CPUs, this metric will be calculated for each CPU. 
* Average Number of Processes in the Ready Queue(s) - The average number of processes waiting in the Ready Queue to be scheduled to the CPU. If there are multiple Ready Queues, this metric will be calculated for each Ready Queue.

### Terminal Output
The simulator displays its results to the terminal. It displays markers when stages of the simulator program (initialization, simulation, statistics, and cleanup) have completed. The simulator outputs 4 metrics for the simulated system. These metrics are stated in the Metrics section.

## Project Status
This project is currently *in-progress*. The current development is focusing on separating out code to follow OOP principles more closely. 

### Planned Additions/Changes
* Add preemption of processes (interrupting their execution partially through running)
* Add more scheduler types (e.g. Round Robin)
* Introduce a GUI for inputing the parameters of the simulator and viewing the output metrics. 
  * Qt is the current forerunner for which framework to use.