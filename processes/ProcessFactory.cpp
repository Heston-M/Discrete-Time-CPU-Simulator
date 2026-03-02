#include "ProcessFactory.h"

ProcessFactory::ProcessFactory(float affinityProbability, int numCPUs, RandomGenerator *randGen) {
  this->affinityProbability = affinityProbability;
  this->numCPUs = numCPUs;
  this->randGen = randGen;
}

ProcessFactory::~ProcessFactory() {
  delete randGen;
}

Process* ProcessFactory::createProcess(float serviceTime, float arrivalTime) {
  Process *process = new Process(serviceTime, arrivalTime);
  if (randGen->getRandomFloat() < affinityProbability) {
    process->affinity = randGen->getRandomIndex(numCPUs);
  }
  return process;
}