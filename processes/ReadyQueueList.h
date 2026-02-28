#ifndef READYQUEUELIST_H
#define READYQUEUELIST_H

#include "Process.h"
#include <vector>

using namespace std;

// ====================================================================
// Ready Queue List structure
class ReadyQueueList {
  private:  
    int schedulerType;
    struct ReadyQueue;
    vector<ReadyQueue *> RQs;
  
  public:
    ReadyQueueList(int, int = 1);
    ~ReadyQueueList();

    void setSchedulerType(int);
  
    int getNumRQs();
    int getRQSize(int = 0);
    bool isRQEmpty(int = 0);
  
    void insertProcessRQ(Process *, int = 0);
    Process* dequeueProcessRQ(float, int = 0);
  };

#endif // READYQUEUELIST_H