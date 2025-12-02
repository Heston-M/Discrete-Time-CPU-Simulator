/*
  Discrete Time Event Simulator
  - created 10/23/2025
  - by Heston Montagne
  - for CS 3360 Homework #4 assignment

  This program creates and runs a discrete time event simulation of a multi-CPU
  queuing system. It simulates processes arriving to the system, possibly
  waiting in a Ready Queue ordered by one of two possible schedulers, 
  running on one of the CPUs, and then leaving the system.

  See README.md for details.
*/


#include "generators/RandomGenerator.h"
#include "generators/TimeGenerator.h"
#include "processes/Process.h"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

// ====================================================================
// PROGRAM CONFIG VARIABLES
const bool PRINT_LIVE_UPDATES = false;  // Print live event updates to console (slows performance)
const int N = 10000;                    // Simulation halts after N processes depart
const bool CHOOSESCHEDULER = false;    // Prompt user to choose scheduler at runtime
const int DEFAULTSCHEDULER = 0;        // Default scheduler if not choosing at runtime (0 = FCFS, 1 = SJF)
const bool CHOOSENUMCPUS = true;     // Prompt user to choose number of CPUs at runtime
const int DEFAULTNUMCPUS = 1;         // Default number of CPUs if not choosing at runtime
const bool CHOOSERQSETUP = true;      // Prompt user to choose Ready Queue setup at runtime
const int DEFAULTRQSETUP = 2;        // Default Ready Queue setup if not choosing at runtime (2 = single global RQ)


// ====================================================================
// GLOBAL VARIABLES Pt. 1
RandomGenerator *randGen;
TimeGenerator *timeGen;
int schedulerType = 0; // 0 = FCFS, 1 = SJF


// ====================================================================
// Event structures
enum EventType {
  ARRIVAL,
  DEPARTURE
};

struct Event {
  float time;
  EventType type;
  Process *process;
  Event *next;
};


// ====================================================================
// CPU List structure 
struct CPUList {
  vector<Process *> CPUs;

  CPUList (int numCPUs = 1) {
    for (int i = 0; i < numCPUs; i++) {
      CPUs.push_back(nullptr);
    }
  }

  ~CPUList() {
    for (int i = 0; i < CPUs.size(); i++) {
      if (CPUs[i]) {
        delete CPUs[i];
      }
    }
  }

  int getNumCPUs() {
    return CPUs.size();
  }

  bool isCPUIdle(int cpuIndex = 0) {
    return CPUs[cpuIndex] == nullptr;
  }

  vector<int> getIdleCPUs() {
    vector<int> idleCPUs;
    for (int i = 0; i < CPUs.size(); i++) {
      if (CPUs[i] == nullptr) {
        idleCPUs.push_back(i);
      }
    }
    return idleCPUs;
  }

  void assignProcessToCPU(Process *process, int cpuIndex = 0) {
    CPUs[cpuIndex] = process;
    if (process) process->CPUindex = cpuIndex;
  }

  Process* getProcessOnCPU(int cpuIndex = 0) {
    return CPUs[cpuIndex];
  }

  Process* removeProcessFromCPU(int cpuIndex = 0) {
    Process *p = CPUs[cpuIndex];
    CPUs[cpuIndex] = nullptr;
    return p;
  }
};


// ====================================================================
// Ready Queue List structure
struct ReadyQueueList {
  struct ReadyQueue {
    Process *head;
    Process *tail;
    int size;

    ReadyQueue() {
      head = nullptr;
      tail = nullptr;
      size = 0;
    }
  };
  vector<ReadyQueue *> RQs;

  ReadyQueueList(int numOfQueues = 1) {
    for (int i = 0; i < numOfQueues; i++) {
      RQs.push_back(new ReadyQueue());
    }
  }

  ~ReadyQueueList() {
    for (int i = 0; i < RQs.size(); i++) {
      Process *p = RQs[i]->head;
      while (p) {
        RQs[i]->head = RQs[i]->head->next;
        delete p;
        p = RQs[i]->head;
      }
    }
    for (int i = 0; i < RQs.size(); i++) {
      delete RQs[i];
    }
  }

  int getNumRQs() {
    return RQs.size();
  }

  int getRQSize(int queueIndex = 0) {
    return RQs[queueIndex]->size;
  }

  bool isRQEmpty(int queueIndex = 0) {
    return RQs[queueIndex]->size == 0;
  }

  // Insert process into the target Ready Queue based on FCFS or SJF
  void insertProcessRQ(Process *process, int queueIndex = 0) {
    ReadyQueue *RQ = RQs[queueIndex];
    if (schedulerType == 0) {              // FCFS
      if (RQs[queueIndex]->head == nullptr) {
        RQ->head = process;
        RQ->tail = process;
      } 
      else {
        RQ->tail->next = process;
        RQ->tail = process;
      }
    }
    else {                                 // SJF
      if (!RQ->head || process->serviceTime < RQ->head->serviceTime) {
        process->next = RQ->head;
        RQ->head = process;
      }
      else {
        Process *p = RQ->head;
        while (p->next && p->next->serviceTime < process->serviceTime) {
          p = p->next;
        }
        process->next = p->next;
        p->next = process;
      }
    }
    RQ->size++;
  }

  Process* getNextProcessRQ(int queueIndex = 0) {
    return RQs[queueIndex]->head;
  }

  Process* removeProcessRQ(int queueIndex = 0) {
    if (isRQEmpty(queueIndex)) {
      throw runtime_error("Error: Attempted to remove process from empty Ready Queue.");
    }
    Process *p = RQs[queueIndex]->head;
    RQs[queueIndex]->head = RQs[queueIndex]->head->next;
    RQs[queueIndex]->size--;
    return p;
  }
};


// ====================================================================
// GLOBAL VARIABLES Pt. 2
Event *eventQHead;
// Process *readyQHead;
// Process *readyQTail;
// Process *CPU;
CPUList *cpuList;
ReadyQueueList *RQList;
// int readyQSize;


// ====================================================================
// Structure to track and calculate statistics about the simulation.
// Tracks average turnaround time, throughput, utilization, and average
//   number of processes in the Ready Queue. 
struct StatisticsUnit {
  float totalTurnTime;
  float numProcessesDone;
  vector<float> utilizationTimes;

  // Variables used for calculating avgProcessesInQ
  struct queueSample {
    int numOfProcesses;
    float sampleTime;
    queueSample *next;
  };
  vector<queueSample *> sampleHeads;
  float sampleTimeDiff;

  // Constructor: initializes to default values and 
  // sets sample time differential based on arrivalRate.
  StatisticsUnit(float arrivalRate) {
    totalTurnTime = 0.0;
    numProcessesDone = 0.0;
    utilizationTimes = vector<float>(cpuList->getNumCPUs(), 0.0);

    sampleHeads = vector<queueSample *>(RQList->getNumRQs(), nullptr);
    sampleTimeDiff = 0.5 / arrivalRate;
  }

  // Destructor: deletes dynamically-allocated memory for sample list
  ~StatisticsUnit() {
    for (int i = 0; i < sampleHeads.size(); i++) {
      queueSample *p = sampleHeads[i];
      while (p) {
        sampleHeads[i] = sampleHeads[i]->next;
        delete p;
        p = sampleHeads[i];
      }
    }
  }

  // Account for a process that has finished at given time. 
  void processDone(Process *process, float time) {
    totalTurnTime += time - process->arrivalTime;
    numProcessesDone++;
    utilizationTimes[process->CPUindex] += process->serviceTime;
  }

  // Samples the Ready Queue at discrete time intervals up to the
  // given time, which should be when the Ready Queue size was updated.
  // Should be called on every update of the Ready Queue size.
  void sampleRQueue(float time, int RQindex = 0) {
    float lastSampleTime = sampleHeads[RQindex] ? sampleHeads[RQindex]->sampleTime : 0.0;
    while (lastSampleTime < time) {
      lastSampleTime += sampleTimeDiff;
      queueSample *p = new queueSample;
      p->numOfProcesses = RQList->getRQSize(RQindex);
      p->next = sampleHeads[RQindex];
      p->sampleTime = lastSampleTime;
      sampleHeads[RQindex] = p;
    }
  }

  // Get the average turnaround time for the system.
  float getAvgTurnTime() {
    return totalTurnTime / numProcessesDone;
  }

  // Get the throughput for the system up to time totalTime.
  float getThroughput(float totalTime) {
    return numProcessesDone / totalTime;
  }

  // Get the average utilization of the system up to time totalTime.
  float getUtilization(float totalTime, int CPUindex = 0) {
    return utilizationTimes[CPUindex] / totalTime;
  }

  // Get the average number of processes in the Ready Queue up to time totalTime. 
  float getAvgProcessesInQ(float totalTime, int RQindex = 0) {
    // Run sample to capture end of simulation data. Extraneous in some cases but doesn't hurt.
    sampleRQueue(totalTime, RQindex); 

    int n = 0;
    float sum = 0;
    queueSample *p = sampleHeads[RQindex];

    while (p) {
      sum += p->numOfProcesses;
      n++;
      p = p->next;
    }
    return n == 0 ? 0.0 : sum / n;
  }
};


// ====================================================================
// GLOBAL VARIABLES Pt. 3
StatisticsUnit *stats;


// ====================================================================
// Inserts new event for an arrival or departure to Event Queue.
void scheduleEvent(EventType type, float t, Process *process) {
  // Create new event
  Event *event = new Event;
  event->type = type;
  event->time = t;
  event->process = process;
  event->next = nullptr;

  // Insert event into Event Queue sorted by accending times
  if (!eventQHead || t < eventQHead->time) {
    event->next = eventQHead;
    eventQHead = event;
  }
  else {
    Event *p = eventQHead;
    while (p->next && p->next->time < t) {
      p = p->next;
    }
    event->next = p->next;
    p->next = event;
  }
}


// Insert process into the Ready Queue based on FCFS or SJF
// void insertReadyQ(Process *process) {
//   if (schedulerType == 0) {              // FCFS
//     if (readyQHead == nullptr) {
//       readyQHead = process;
//       readyQTail = process;
//     } 
//     else {
//       readyQTail->next = process;
//       readyQTail = process;
//     }
//   }
//   else {                                 // SJF
//     if (!readyQHead || process->serviceTime < readyQHead->serviceTime) {
//       process->next = readyQHead;
//       readyQHead = process;
//     }
//     else {
//       Process *p = readyQHead;
//       while (p->next && p->next->serviceTime < process->serviceTime) {
//         p = p->next;
//       }
//       process->next = p->next;
//       p->next = process;
//     }
//   }
//   readyQSize++;
// }


// ====================================================================
// Handle an arrival event (process arrives to system)
// Generates next arrival based on clock.
// Assigns e's process to the CPU (if idle), or inserts it into the Ready Queue.
void handleArrival(Event *e, float clock) {
  float nextArrivalTime = clock + timeGen->getInterArrivalTime();
  scheduleEvent(ARRIVAL, nextArrivalTime, new Process(nextArrivalTime)); // Next arrival

  if (PRINT_LIVE_UPDATES) cout << "Process " << e->process->id << " arrived. ";

  int CPUindex = 0;
  int RQindex = 0;
  if (RQList->getNumRQs() == 1) {   // Single Ready Queue setup
    vector<int> idleCPUs = cpuList->getIdleCPUs();
    if (idleCPUs.size() > 0) {
      CPUindex = idleCPUs[randGen->getRandomIndex(idleCPUs.size())];  // Pick random idle CPU
    }
  }
  else {                            // Per-CPU Ready Queue setup
    CPUindex = randGen->getRandomIndex(cpuList->getNumCPUs());
    RQindex = CPUindex;
  }
  
  if (cpuList->isCPUIdle(CPUindex)) {            // Target CPU is idle
    cpuList->assignProcessToCPU(e->process, CPUindex);
    scheduleEvent(DEPARTURE, clock + e->process->serviceTime, e->process);
    if (PRINT_LIVE_UPDATES) cout << "CPU " << CPUindex << " was idle, so process " << e->process->id 
      << " (" << e->process->serviceTime << ") started running on CPU " << CPUindex << ". ";
  }
  else {                                         // Target CPU is busy, add to its Ready Queue
    RQList->insertProcessRQ(e->process, RQindex);
    stats->sampleRQueue(clock, RQindex);
    if (PRINT_LIVE_UPDATES) {
      int RQsize = RQList->getRQSize(RQindex);
      if (RQList->getNumRQs() == 1) cout << "No CPU was idle, so the process was added to Ready Queue (" << RQsize << "). ";
      else cout << "CPU " << CPUindex << " was busy, so the process was added to Ready Queue " << RQindex << " (" << RQsize << "). ";
    }
  }



  // if (RQList->getNumRQs() == 1) {   // Single Ready Queue setup
  //   if (RQList->isRQEmpty(0)) {     // Ready Queue is empty
  //     vector<int> idleCPUs = cpuList->getIdleCPUs();
  //     if (idleCPUs.size() == 0) {   // No CPU is idle, add to Ready Queue
  //       RQList->insertProcessRQ(0, e->process);
  //       stats->sampleRQueue(clock);                                                 // SAMPLE READY QUEUE WILL NEED TO BE MODIFIED FOR MULTIPLE RQS
  //       if (PRINT_LIVE_UPDATES) cout << "No CPU was idle, so the process was added to Ready Queue. ";
  //     }
  //     else {                        // At least one CPU is idle, assign to one
  //       int CPUindex = idleCPUs[randGen->getRandomIndex(idleCPUs.size())];  // Pick random idle CPU
  //       cpuList->assignProcessToCPU(CPUindex, e->process);
  //       scheduleEvent(DEPARTURE, clock + e->process->serviceTime, e->process);
  //       if (PRINT_LIVE_UPDATES) cout << "CPU " << CPUindex << " was idle, so process " << e->process->id 
  //         << " (" << e->process->serviceTime << ") started running on CPU" << CPUindex << ". ";
  //     }
  //   }
  //   else {                          // Ready Queue is not empty, add to it
  //     RQList->insertProcessRQ(0, e->process);
  //     stats->sampleRQueue(clock);
  //     if (PRINT_LIVE_UPDATES) cout << "Ready Queue was not empty, so the process was added to Ready Queue. ";
  //   }
  // } 
  // else {                            // Per-CPU Ready Queue setup
  //   int CPUindex = randGen->getRandomIndex(cpuList->getNumCPUs());  // Pick random CPU
  //   if (cpuList->isCPUIdle(CPUindex)) {         // Chosen CPU is idle
  //     cpuList->assignProcessToCPU(CPUindex, e->process);
  //     scheduleEvent(DEPARTURE, clock + e->process->serviceTime, e->process);
  //     if (PRINT_LIVE_UPDATES) cout << "CPU " << CPUindex << " was idle, so process " << e->process->id 
  //       << " (" << e->process->serviceTime << ") started running on CPU" << CPUindex << ". ";
  //   }
  //   else {                                      // Chosen CPU is busy, add to its Ready Queue
  //     RQList->insertProcessRQ(CPUindex, e->process);
  //     stats->sampleRQueue(clock);
  //     if (PRINT_LIVE_UPDATES) cout << "CPU " << CPUindex << " was busy, so the process was added to Ready Queue " << CPUindex << ". ";
  //   }
  // }


  // if (CPU) {  
  //   insertReadyQ(e->process);    // CPU is busy, add to Ready Queue
  //   stats->sampleRQueue(clock);
  //   if (PRINT_LIVE_UPDATES) cout << "CPU was busy, so the process was added to Ready Queue. ";
  // }
  // else {
  //   if (schedulerType == 0 || !readyQHead || e->process->serviceTime < readyQHead->serviceTime) {
  //     CPU = e->process;       // New process goes to CPU
  //   }
  //   else {
  //     insertReadyQ(e->process);
  //     CPU = readyQHead;
  //     readyQHead->next;
  //     readyQSize--;
  //   }
  //   scheduleEvent(DEPARTURE, clock + CPU->serviceTime, CPU);
  //   if (PRINT_LIVE_UPDATES) cout << "CPU was idle, so process " << CPU->id 
  //     << " (" << CPU->serviceTime << ") started running on CPU. ";
  // }
}


// ====================================================================
// Handle a departure event (process is finished on CPU)
// Deletes e.
// Next process is pulled from the Ready Queue, or the CPU goes idle if empty.
void handleDeparture(Event *e, float clock) {
  stats->processDone(e->process, clock);

  int CPUindex = e->process->CPUindex;
  if (cpuList->getProcessOnCPU(CPUindex)->id != e->process->id) {
    throw runtime_error("Error: Process on CPU does not match departing process.");
  }

  int RQindex = 0;
  if (RQList->getNumRQs() != 1) {             // Get correct Ready Queue index
    RQindex = CPUindex;
  }

  cpuList->removeProcessFromCPU(CPUindex);
  if (PRINT_LIVE_UPDATES) cout << "Process " << e->process->id << " departed from CPU " << CPUindex << ". ";

  if (RQList->isRQEmpty(RQindex)) {           // Target Ready Queue is empty
    if (PRINT_LIVE_UPDATES) cout << "CPU " << CPUindex << " is now idle. ";
  }
  else {                                      // Target Ready Queue is not empty, move next process to target CPU
    Process *nextProcess = RQList->removeProcessRQ(RQindex);
    cpuList->assignProcessToCPU(nextProcess, CPUindex);
    stats->sampleRQueue(clock, RQindex);
    scheduleEvent(DEPARTURE, clock + nextProcess->serviceTime, nextProcess);
    if (PRINT_LIVE_UPDATES) cout << "Process " << nextProcess->id 
      << " (" << nextProcess->serviceTime << ") moving to CPU " << CPUindex << ". ";
  }


  // if (readyQHead) {
  //   CPU = readyQHead;          // Ready Queue is populated
  //   readyQHead = readyQHead->next;
  //   readyQSize--;
  //   stats->sampleRQueue(clock);
  //   scheduleEvent(DEPARTURE, clock + CPU->serviceTime, CPU);
  //   if (PRINT_LIVE_UPDATES) cout << "Process " << CPU->id 
  //     << " (" << CPU->serviceTime << ") moving to CPU. ";
  // }
  // else {
  //   CPU = nullptr;             // Ready Queue is empty
  //   if (PRINT_LIVE_UPDATES) cout << "CPU is now idle. ";
  // }

  delete e->process;
}


// ====================================================================
int main() {

  // ======================
  // INITIALIZATION
  // ======================

  // User arguments
  float arrivalLambda;
  float serviceTimeAvg;
  int numCPUs;
  int rqSetup;

  cout << "\n========================================";
  cout << "\n     DISCRETE TIME EVENT SIMULATOR      ";
  cout << "\n          - Heston Montagne -           ";
  cout << "\n========================================\n";

  cout << "\nEnter the average arrival rate (processes per second): ";
  cin >> arrivalLambda;

  cout << "\nEnter the average service time (seconds): ";
  cin >> serviceTimeAvg;

  if (CHOOSESCHEDULER) {
    cout << "\nPick the scheduler (0 for FCFS, 1 for SJF): ";
    cin >> schedulerType;
  } else {
    schedulerType = DEFAULTSCHEDULER;
  }

  if (CHOOSERQSETUP) {
    cout << "\nPick the Ready Queue setup (1 for RQ per-CPU, 2 for single global RQ): ";
    cin >> rqSetup;
  } else {
    rqSetup = DEFAULTRQSETUP;
  }

  if (CHOOSENUMCPUS) {
    cout << "\nEnter the number of CPUs: ";
    cin >> numCPUs;
  } else {
    numCPUs = DEFAULTNUMCPUS;
  }

  cout << "\n========================================";

  if (arrivalLambda <= 0 || serviceTimeAvg <= 0 || !(schedulerType == 0 || schedulerType == 1) || numCPUs <= 0 || !(rqSetup == 1 || rqSetup == 2)) {
    throw runtime_error("Invalid arguments.");
  }

  if (rqSetup == 1) {
    randGen = new RandomGenerator();
  } else {
    randGen = nullptr;
  }

  // Initialize time generation with lambdas
  timeGen = new TimeGenerator(arrivalLambda, serviceTimeAvg);

  eventQHead = nullptr;

  // CPU = nullptr;
  cpuList = new CPUList(numCPUs);
  if (rqSetup == 2) {
    RQList = new ReadyQueueList(1);
  } else {
    RQList = new ReadyQueueList(numCPUs);
  }

  stats = new StatisticsUnit(arrivalLambda);

  float clock = 0.0; // Current time tracker

  Process *firstProcess = new Process(clock);
  firstProcess->setTimeGen(timeGen);
  scheduleEvent(ARRIVAL, clock + timeGen->getInterArrivalTime(), firstProcess);

  cout << "\n\nInitialization Complete\n";

  // ======================
  // SIMULATION
  // ======================
  
  int departures = 0;

  while (eventQHead && departures < N) {
    Event *event = eventQHead;

    float oldClock = clock;
    clock = event->time;
    float timeDiff = clock - oldClock;

    if (PRINT_LIVE_UPDATES) cout << "T = " << fixed << setprecision(4) << clock << " | ";

    switch (event->type) {
      case ARRIVAL: 
        handleArrival(event, clock);
        break;

      case DEPARTURE:
        handleDeparture(event, clock);
        departures++;
        break;

      default: 
        throw runtime_error("Encountered invalid event type.");
    }

    if (PRINT_LIVE_UPDATES) cout << "\n";

    eventQHead = eventQHead->next;
    delete event;
  }

  cout << "\nSimulation Complete\n\n";

  // ======================
  // STATISTICS
  // ======================

  cout << "========================================\n";
  cout << "\nAverage Turnaround Time: " << stats->getAvgTurnTime()      << " seconds.\n";
  cout << "\nTotal Throughput: "        << stats->getThroughput(clock)  << " processes per second.\n";

  if (cpuList->getNumCPUs() == 1) cout << "\nCPU Utilization: "         << stats->getUtilization(clock) << ".\n";
  else {
    cout << "\nCPU Utilizations: \n";
    for (int i = 0; i < cpuList->getNumCPUs(); i++) {
      cout << "    CPU " << i << " Utilization: " << stats->getUtilization(clock, i) << ".\n";
    }
  }

  if (RQList->getNumRQs() == 1) cout << "\nAverage Number of Processes in the Ready Queue: " << stats->getAvgProcessesInQ(clock) << " processes.\n";
  else {
    cout << "\nAverage Number of Processes in Each Ready Queue: \n";
    for (int i = 0; i < RQList->getNumRQs(); i++) {
      cout << "    Ready Queue " << i << ": " << stats->getAvgProcessesInQ(clock, i) << " processes.\n";
    }
  }

  cout << "\n========================================\n";

  cout << "\nStatistics Complete\n\n";

  // ======================
  // CLEANUP
  // ======================

  delete timeGen;
  delete randGen;
  delete stats;
  delete cpuList;
  delete RQList;

  Event *e = eventQHead;
  while (eventQHead) {
    eventQHead = eventQHead->next;
    delete e->process;
    delete e;
    e = eventQHead;
  }

  cout << "Cleanup Complete\n";
  cout << "\n========================================\n\n";

  return 1;
}