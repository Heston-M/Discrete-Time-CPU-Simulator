#include "Process.h"

int Process::nextID = 0;
TimeGenerator *Process::timeGen = nullptr;

Process::Process(float t) {
  id = nextID;
  serviceTime = timeGen->getServiceTime();
  arrivalTime = t;
  departureTime = 0;
  next = nullptr;

  nextID++;
}