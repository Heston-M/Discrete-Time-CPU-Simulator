#ifndef PROCESS_H
#define PROCESS_H

// ====================================================================
// Process structure
class Process {

  private:
    static int nextID;

  public:
    int id;
    float serviceTime;
    float arrivalTime;
    float departureTime;
    int CPUindex;
    int RQindex;
    Process *next;
  
    Process(float, float);
  };

#endif // PROCESS_H