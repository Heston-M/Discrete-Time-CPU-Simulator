#include "Process.h"

int Process::nextID = 0;

Process::Process(float serviceT, float arrivalT) {
  id = nextID;
  serviceTime = serviceT;
  timeLeft = serviceTime;
  arrivalTime = arrivalT;
  lastRunTime = 0;
  departureTime = 0;
  CPUindex = -1;
  RQindex = -1;
  next = nullptr;

  nextID++;
}