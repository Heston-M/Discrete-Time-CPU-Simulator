#ifndef PROCESSFACTORY_H
#define PROCESSFACTORY_H

#include "Process.h"
#include "../generators/RandomGenerator.h"

class ProcessFactory {
  private:
    float affinityProbability;
    int numCPUs;

    RandomGenerator *randGen;

  public:
    ProcessFactory(float, int, RandomGenerator*);
    ~ProcessFactory();

    Process* createProcess(float, float);
  };

#endif // PROCESSFACTORY_H