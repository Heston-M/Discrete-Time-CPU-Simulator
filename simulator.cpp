/*
  Discrete Time Event Simulator
  - created 10/23/2025
  - by Heston Montagne

  This program creates and runs a discrete time event simulation of a multi-CPU
  queuing system. It simulates processes arriving to the system, possibly
  waiting in a Ready Queue ordered by one of two possible schedulers, 
  running on one of the CPUs, and then leaving the system.

  See README.md for details.
*/


#include "endChecker/endChecker.h"
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
  scheduleEvent(ARRIVAL, nextArrivalTime, new Process(timeGen->getServiceTime(), nextArrivalTime)); // Next arrival

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

  cout << "\n========================================";
  cout << "\n     DISCRETE TIME EVENT SIMULATOR      ";
  cout << "\n          - Heston Montagne -           ";
  cout << "\n========================================\n\n";

  // ======================
  // INITIALIZATION
  // ======================

  // User arguments
  float arrivalLambda;
  float serviceTimeAvg;
  int numCPUs;
  int rqSetup;

  arrivalLambda = InputHandler::getArrivalLambda();
  serviceTimeAvg = InputHandler::getServiceTimeAvg();
  schedulerType = InputHandler::getSchedulerType();
  rqSetup = InputHandler::getRQSetup();
  numCPUs = InputHandler::getNumCPUs();

  EndChecker endChecker;

  if (arrivalLambda <= 0 || serviceTimeAvg <= 0 || !(schedulerType == 0 || schedulerType == 1) || numCPUs <= 0 || !(rqSetup == 1 || rqSetup == 2)) {
    throw runtime_error("Invalid arguments.");
  }

  int numRQs = rqSetup == 2 ? 1 : numCPUs;

  randGen = new RandomGenerator();
  timeGen = new TimeGenerator(arrivalLambda, serviceTimeAvg);
  cpuList = new CPUList(numCPUs);
  RQList = new ReadyQueueList(numRQs, schedulerType);
  stats = new StatisticsUnit(arrivalLambda, cpuList, RQList);

  float clock = 0.0; // Current time tracker

  // Create first process
  Process *firstProcess = new Process(timeGen->getServiceTime(), clock);
  scheduleEvent(ARRIVAL, firstProcess->arrivalTime, firstProcess);

  cout << "========================================";
  cout << "\n\nInitialization Complete\n";
  cout << "\n========================================\n";

  // ======================
  // SIMULATION
  // ======================

  while (!endChecker.checkEnd()) {
    Event *event = eventQHead;
    if (!event) {
      throw runtime_error("Error: Event queue is empty.");
    }

    float oldClock = clock;
    clock = event->time;
    float timeDiff = clock - oldClock;

    if (PRINT_LIVE_UPDATES) cout << "T = " << fixed << setprecision(4) << clock << " | ";

    switch (event->type) {
      case ARRIVAL: 
        handleArrival(event, clock);
        endChecker.logArrival(clock);
        break;

      case DEPARTURE:
        handleDeparture(event, clock);
        endChecker.logDeparture(clock);
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

  return 0;
}