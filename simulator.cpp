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
#include "input/InputHandler.h"
#include "processes/Process.h"
#include "processes/ReadyQueueList.h"
#include "processes/CPUList.h"
#include "statistics/StatisticsUnit.h"
#include "config.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

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
// GLOBAL VARIABLES
int schedulerType = 0; // 0 = FCFS, 1 = SJF
Event *eventQHead = nullptr;

RandomGenerator *randGen = nullptr;
TimeGenerator *timeGen = nullptr;
StatisticsUnit *stats = nullptr;

CPUList *cpuList = nullptr;
ReadyQueueList *RQList = nullptr;


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
  cout << "\n========================================\n\n";

  InputHandler inputHandler;
  inputHandler.handleInput();

  arrivalLambda = inputHandler.getArrivalLambda();
  serviceTimeAvg = inputHandler.getServiceTimeAvg();
  schedulerType = inputHandler.getSchedulerType();
  rqSetup = inputHandler.getRQSetup();
  numCPUs = inputHandler.getNumCPUs();

  cout << "========================================";

  if (arrivalLambda <= 0 || serviceTimeAvg <= 0 || !(schedulerType == 0 || schedulerType == 1) || numCPUs <= 0 || !(rqSetup == 1 || rqSetup == 2)) {
    throw runtime_error("Invalid arguments.");
  }

  if (rqSetup == 1) randGen = new RandomGenerator();
  else randGen = nullptr;

  timeGen = new TimeGenerator(arrivalLambda, serviceTimeAvg);

  cpuList = new CPUList(numCPUs);

  if (rqSetup == 2) RQList = new ReadyQueueList(1);
  else RQList = new ReadyQueueList(numCPUs);
  RQList->setSchedulerType(schedulerType);

  stats = new StatisticsUnit(arrivalLambda, cpuList, RQList);

  float clock = 0.0; // Current time tracker

  // Create first process
  Process *firstProcess = new Process(clock);
  firstProcess->setTimeGen(timeGen);
  scheduleEvent(ARRIVAL, clock + timeGen->getInterArrivalTime(), firstProcess);

  cout << "\n\nInitialization Complete\n";
  cout << "\n========================================\n";

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
  cout << "========================================\n\n";

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