#ifndef OUTPUT_H
#define OUTPUT_H

#include "../processes/Process.h"
#include "../processes/ReadyQueueList.h"
#include <vector>
#include <string>

using namespace std;

class Output {
  public:
    enum MetricType {
      AVG_TURN_TIME,
      TOTAL_THROUGHPUT,
      CPU_UTILIZATION,
      AVG_PROCESSES_IN_Q,
    };

    enum LiveUpdateType {
      ARRIVAL_TO_CPU,
      ARRIVAL_TO_RQ,
      DEPARTURE_CPU_IDLE,
      DEPARTURE_NEXT_PROCESS,
      PREEMPTION_INTERVAL
    };

    virtual void printTitle() = 0;
    virtual void printHeader(string message) = 0;
    virtual void printMetric(MetricType metricType, vector<float> values) = 0;
    virtual void printLiveUpdate(float clock, LiveUpdateType eventType, Process *process, ReadyQueueList *RQList, Process *nextProcess = nullptr) = 0;

    virtual ~Output() = default;
};

#endif // OUTPUT_H