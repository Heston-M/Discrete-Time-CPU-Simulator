#ifndef PROCESS_H
#define PROCESS_H

#include "../generators/TimeGenerator.h"

// ====================================================================
// Process structure
class Process {

  private:
    static int nextID;
    static TimeGenerator *timeGen;

  public:
    int id;
    float serviceTime;
    float arrivalTime;
    float departureTime;
    int CPUindex;
    Process *next;
  
    Process(float);

    void setTimeGen(TimeGenerator *tg) {
      timeGen = tg;
    }
  };

#endif // PROCESS_H