#include "Process.h"

int Process::nextID = 0;

Process::Process(float serviceT, float arrivalT) {
  id = nextID;
  serviceTime = serviceT;
  arrivalTime = arrivalT;
  departureTime = 0;
  next = nullptr;

  nextID++;
}