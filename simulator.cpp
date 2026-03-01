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
#include <vector>
#include <stdexcept>

using namespace std;


// ====================================================================
// Event structures
enum EventType {
  ARRIVAL,
  DEPARTURE,
  PREEMPTION
};

struct Event {
  float time;
  EventType type;
  Process *process;
  Event *next;
};


// ====================================================================
// GLOBAL VARIABLES
int schedulerType = 0; // 0 = FCFS, 1 = SJF, 2 = SRTF
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
// Finds and deletes an event from the Event Queue.
// Returns true if event was found and deleted, false otherwise.
bool findAndDeleteEvent(EventType type, Process *target) {
  Event *prev = nullptr;
  Event *current = eventQHead;
  while (current) {
    if (current->type == type && current->process == target) {
      if (prev) {
        prev->next = current->next;
      }
      else {
        eventQHead = current->next;
      }
      delete current;
      return true;
    }
    prev = current;
    current = current->next;
  }
  return false;
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
  Process *currentProcess = nullptr;
  
  if (cpuList->isCPUIdle(CPUindex)) {            // Target CPU is idle
    e->process->RQindex = RQindex;
    cpuList->assignProcessToCPU(clock, e->process, CPUindex);
    scheduleEvent(DEPARTURE, clock + e->process->serviceTime, e->process);
    eventType = Output::ARRIVAL_TO_CPU;
  }
  else {     // Target CPU is busy, add to its Ready Queue, but check for preemption
    RQList->insertProcessRQ(e->process, RQindex);
    stats->sampleRQueue(clock, RQindex);

    currentProcess = cpuList->getProcessOnCPU(CPUindex);
    float timeLeft = currentProcess->timeLeft - (clock - currentProcess->lastRunTime);
    if (schedulerType == 2 && e->process->serviceTime < timeLeft) {  // SRTF & preempt process on CPU
      e->process->CPUindex = CPUindex;
      scheduleEvent(PREEMPTION, clock, currentProcess);
      eventType = Output::ARRIVAL_PREEMPT_SRTF;
    }
    else {
      eventType = Output::ARRIVAL_TO_RQ;
    }
  }

  if (PRINT_LIVE_UPDATES) out->printLiveUpdate(clock, eventType, e->process, RQList, currentProcess);
}


// ====================================================================
// Handle a departure event (process is finished on CPU)
// Deletes e's process.
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

  cpuList->removeProcessFromCPU(clock, CPUindex);

  if (RQList->isRQEmpty(RQindex)) {           // Target Ready Queue is empty
    eventType = Output::DEPARTURE_CPU_IDLE;
  }
  else {                                      // Target Ready Queue is not empty, move next process to target CPU
    nextProcess = RQList->dequeueProcessRQ(clock, RQindex);
    cpuList->assignProcessToCPU(clock, nextProcess, CPUindex);
    stats->sampleRQueue(clock, RQindex);
    scheduleEvent(DEPARTURE, clock + nextProcess->serviceTime, nextProcess);
    eventType = Output::DEPARTURE_NEXT_PROCESS;
  }

  if (PRINT_LIVE_UPDATES) out->printLiveUpdate(clock, eventType, e->process, RQList, nextProcess);

  delete e->process;
}


// ====================================================================
// Handle a preemption event (process is interrupted while running on CPU)
// Puts e's process back into the Ready Queue and puts next process on CPU.
// Puts the process back on the CPU if the Ready Queue is empty. 
void handlePreemption(Event *e, float clock) {
  int CPUindex = e->process->CPUindex;
  if (e->process->id != cpuList->getProcessOnCPU(CPUindex)->id) {
    throw runtime_error("Error: Process on CPU does not match preempted process.");
  }

  int RQindex = 0;
  if (RQList->getNumRQs() != 1) {
    RQindex = CPUindex;
  }

  Process *process = cpuList->removeProcessFromCPU(clock, CPUindex);    // Put process into Ready Queue
  findAndDeleteEvent(DEPARTURE, process);
  RQList->insertProcessRQ(process, RQindex);

  Process *nextProcess = RQList->dequeueProcessRQ(clock, RQindex);      // Move next process in RQ to CPU
  cpuList->assignProcessToCPU(clock, nextProcess, CPUindex);
  scheduleEvent(DEPARTURE, clock + nextProcess->timeLeft, nextProcess);

  bool arrivalPreempt = nextProcess->arrivalTime == clock;
  if (PRINT_LIVE_UPDATES && !arrivalPreempt) out->printLiveUpdate(clock, Output::PREEMPTION_INTERVAL, process, RQList, nextProcess);
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

  arrivalLambda = InputHandler::getInput<float>(InputHandler::ARRIVAL_RATE);
  serviceTimeAvg = InputHandler::getInput<float>(InputHandler::SERVICE_TIME);
  schedulerType = InputHandler::getInput<int>(InputHandler::SCHEDULER);
  rqSetup = InputHandler::getInput<int>(InputHandler::RQ_SETUP);
  numCPUs = InputHandler::getInput<int>(InputHandler::NUM_CPUS);

  EndChecker endChecker;

  if (arrivalLambda <= 0 || serviceTimeAvg <= 0 || (schedulerType < 0 || 3 < schedulerType) || numCPUs <= 0 || !(rqSetup == 1 || rqSetup == 2)) {
    throw runtime_error("Invalid user-input arguments.");
  }

  int numRQs = rqSetup == 2 ? 1 : numCPUs;

  randGen = new RandomGenerator();
  timeGen = new TimeGenerator(arrivalLambda, serviceTimeAvg);
  cpuList = new CPUList(numCPUs);
  RQList = new ReadyQueueList(schedulerType, numRQs);
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

      case PREEMPTION:
        handlePreemption(event, clock);
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
    if (e->type == ARRIVAL) delete e->process;
    delete e;
    e = eventQHead;
  }

  out->printHeader("Cleanup Complete");

  delete out;

  return 0;
}