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
    ReadyQueueList(int = 1, int = 0);
    ~ReadyQueueList();

    void setSchedulerType(int);
  
    int getNumRQs();
    int getRQSize(int);
    bool isRQEmpty(int);
  
    void insertProcessRQ(Process *, int);
    Process* getNextProcessRQ(int);
    Process* removeProcessRQ(int);
  };

#endif // READYQUEUELIST_H