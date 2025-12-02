#ifndef STATISTICSUNIT_H
#define STATISTICSUNIT_H

#include "../processes/Process.h"
#include "../processes/ReadyQueueList.h"
#include "../processes/CPUList.h"
#include <vector>

using namespace std;

// ====================================================================
// Structure to track and calculate statistics about the simulation.
// Tracks average turnaround time, throughput, utilization, and average
//   number of processes in the Ready Queue. 
class StatisticsUnit {
  private:
    CPUList *cpuList;
    ReadyQueueList *RQList;

    float totalTurnTime;
    float numProcessesDone;
    vector<float> utilizationTimes;
  
    // Variables used for calculating avgProcessesInQ
    struct queueSample;
    vector<queueSample *> sampleHeads;
    float sampleTimeDiff;
  
  
  public:
    StatisticsUnit(float, CPUList *, ReadyQueueList *);
    ~StatisticsUnit();
  
    void processDone(Process *, float);

    void sampleRQueue(float, int);

    float getAvgTurnTime();
    float getThroughput(float);
    float getUtilization(float, int = 0);
    float getAvgProcessesInQ(float, int = 0);
};

#endif // STATISTICSUNIT_H