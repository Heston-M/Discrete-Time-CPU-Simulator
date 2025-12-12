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
#include "output/output.h"
#include "output/terminalOutput.h"
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

Output *out = nullptr;


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

  Output::LiveUpdateType eventType;
  
  if (cpuList->isCPUIdle(CPUindex)) {            // Target CPU is idle
    cpuList->assignProcessToCPU(e->process, CPUindex);
    scheduleEvent(DEPARTURE, clock + e->process->serviceTime, e->process);
    eventType = Output::ARRIVAL_TO_CPU;
  }
  else {                                         // Target CPU is busy, add to its Ready Queue
    RQList->insertProcessRQ(e->process, RQindex);
    stats->sampleRQueue(clock, RQindex);
    eventType = Output::ARRIVAL_TO_RQ;
  }

  if (PRINT_LIVE_UPDATES) out->printLiveUpdate(clock, eventType, e->process, RQList);
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

  Output::LiveUpdateType eventType;
  Process *nextProcess = nullptr;

  cpuList->removeProcessFromCPU(CPUindex);

  if (RQList->isRQEmpty(RQindex)) {           // Target Ready Queue is empty
    eventType = Output::DEPARTURE_CPU_IDLE;
  }
  else {                                      // Target Ready Queue is not empty, move next process to target CPU
    nextProcess = RQList->removeProcessRQ(RQindex);
    cpuList->assignProcessToCPU(nextProcess, CPUindex);
    stats->sampleRQueue(clock, RQindex);
    scheduleEvent(DEPARTURE, clock + nextProcess->serviceTime, nextProcess);
    eventType = Output::DEPARTURE_NEXT_PROCESS;
  }

  if (PRINT_LIVE_UPDATES) out->printLiveUpdate(clock, eventType, e->process, RQList, nextProcess);

  delete e->process;
}


// ====================================================================
int main() {
  out = new TerminalOutput();

  out->printTitle();

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

  out->printHeader("Initialization Complete");

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

    eventQHead = eventQHead->next;
    delete event;
  }

  out->printHeader("Simulation Complete");

  // ======================
  // STATISTICS
  // ======================

  out->printMetric(Output::AVG_TURN_TIME, {stats->getAvgTurnTime()});

  out->printMetric(Output::TOTAL_THROUGHPUT, {stats->getThroughput(clock)});

  vector<float> utilizationValues;
  for (int i = 0; i < cpuList->getNumCPUs(); i++) {
    utilizationValues.push_back(stats->getUtilization(clock, i));
  }
  out->printMetric(Output::CPU_UTILIZATION, utilizationValues);

  vector<float> processesInQValues;
  for (int i = 0; i < RQList->getNumRQs(); i++) {
    processesInQValues.push_back(stats->getAvgProcessesInQ(clock, i));
  }
  out->printMetric(Output::AVG_PROCESSES_IN_Q, processesInQValues);


  out->printHeader("Statistics Complete");

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

  out->printHeader("Cleanup Complete");

  delete out;

  return 0;
}