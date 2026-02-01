#ifndef CPULIST_H
#define CPULIST_H

#include "Process.h"
#include <vector>

using namespace std;

// ====================================================================
// CPU List structure 
class CPUList {
  private:
    vector<Process *> CPUs;
  
  public:
    CPUList (int = 1);
    ~CPUList();
  
    int getNumCPUs();
    bool isCPUIdle(int);
    vector<int> getIdleCPUs();
  
    void assignProcessToCPU(float, Process *, int);
    Process* getProcessOnCPU(int);
    Process* removeProcessFromCPU(float, int);
  };

#endif // CPULIST_H