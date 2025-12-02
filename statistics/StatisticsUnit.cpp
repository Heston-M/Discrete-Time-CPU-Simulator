#include "StatisticsUnit.h"

// Variables used for calculating avgProcessesInQ
struct StatisticsUnit::queueSample {
  int numOfProcesses;
  float sampleTime;
  queueSample *next;
};

// Constructor: initializes to default values and 
// sets sample time differential based on arrivalRate.
StatisticsUnit::StatisticsUnit(float arrivalRate, CPUList *cpuList, ReadyQueueList *RQList) {
  this->cpuList = cpuList;
  this->RQList = RQList;
  
  totalTurnTime = 0.0;
  numProcessesDone = 0.0;

  utilizationTimes = vector<float>(cpuList->getNumCPUs(), 0.0);
  sampleHeads = vector<queueSample *>(RQList->getNumRQs(), nullptr);
  sampleTimeDiff = 0.5 / arrivalRate;
}

// Destructor: deletes dynamically-allocated memory for sample list
StatisticsUnit::~StatisticsUnit() {
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
void StatisticsUnit::processDone(Process *process, float time) {
  totalTurnTime += time - process->arrivalTime;
  numProcessesDone++;
  utilizationTimes[process->CPUindex] += process->serviceTime;
}

// Samples the Ready Queue at discrete time intervals up to the
// given time, which should be when the Ready Queue size was updated.
// Should be called on every update of the Ready Queue size.
void StatisticsUnit::sampleRQueue(float time, int RQindex = 0) {
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
float StatisticsUnit::getAvgTurnTime() {
  return totalTurnTime / numProcessesDone;
}

// Get the throughput for the system up to time totalTime.
float StatisticsUnit::getThroughput(float totalTime) {
  return numProcessesDone / totalTime;
}

// Get the average utilization of the system up to time totalTime.
float StatisticsUnit::getUtilization(float totalTime, int CPUindex) {
  return utilizationTimes[CPUindex] / totalTime;
}

// Get the average number of processes in the Ready Queue up to time totalTime. 
float StatisticsUnit::getAvgProcessesInQ(float totalTime, int RQindex) {
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