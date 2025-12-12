#ifndef OUTPUT_H
#define OUTPUT_H

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

    virtual void printTitle() = 0;
    virtual void printHeader(string message) = 0;
    virtual void printMetric(MetricType metricType, vector<float> values) = 0;

    virtual ~Output() = default;
};

#endif // OUTPUT_H