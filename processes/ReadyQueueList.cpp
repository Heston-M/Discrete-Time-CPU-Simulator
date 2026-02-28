#include "ReadyQueueList.h"
#include <stdexcept>

struct ReadyQueueList::ReadyQueue {
  Process *head;
  Process *tail;
  int size;

  ReadyQueue() {
    head = nullptr;
    tail = nullptr;
    size = 0;
  }
};

ReadyQueueList::ReadyQueueList(int schedulerType, int numOfQueues) {
  this->schedulerType = schedulerType;
  for (int i = 0; i < numOfQueues; i++) {
    RQs.push_back(new ReadyQueue());
  }
}

ReadyQueueList::~ReadyQueueList() {
  for (int i = 0; i < RQs.size(); i++) {
    Process *p = RQs[i]->head;
    while (p) {
      RQs[i]->head = RQs[i]->head->next;
      delete p;
      p = RQs[i]->head;
    }
  }
  for (int i = 0; i < RQs.size(); i++) {
      delete RQs[i];
  }
}

void ReadyQueueList::setSchedulerType(int st) {
  schedulerType = st;
}

int ReadyQueueList::getNumRQs() {
  return RQs.size();
}

int ReadyQueueList::getRQSize(int queueIndex) {
  return RQs[queueIndex]->size;
}

bool ReadyQueueList::isRQEmpty(int queueIndex) {
  return RQs[queueIndex]->size == 0;
}

// Insert process into the target Ready Queue based on FCFS, SJF, SRTF, or HRRN
void ReadyQueueList::insertProcessRQ(Process *process, int queueIndex) {
  process->RQindex = queueIndex;
  ReadyQueue *RQ = RQs[queueIndex];

  if (schedulerType == 1 || schedulerType == 2) {      // SJF & SRTF
    if (!RQ->head || process->timeLeft < RQ->head->timeLeft) {
      process->next = RQ->head;
      RQ->head = process;
    }
    else {
      Process *p = RQ->head;
      while (p->next && p->next->timeLeft < process->timeLeft) {
          p = p->next;
      }
      process->next = p->next;
      p->next = process;
    }
  }
  else {                                               // FCFS or HRRN or default
    if (RQs[queueIndex]->head == nullptr) {
      RQ->head = process;
      RQ->tail = process;
    } 
    else {
      RQ->tail->next = process;
      RQ->tail = process;
    }
  }
  RQ->size++;
}

Process* ReadyQueueList::dequeueProcessRQ(float clock, int queueIndex) {
  if (isRQEmpty(queueIndex)) {
      throw runtime_error("Error: Attempted to remove process from empty Ready Queue.");
  }
  if (schedulerType == 3) {                      // HRRN, dequeue process with highest RR
    float maxRR = 0.0;
    int maxRRID = -1;

    Process *p = RQs[queueIndex]->head;
    while (p) {                                  // Find process with highest RR
      float RR = 1 + (clock - p->arrivalTime) / p->serviceTime;
      if (RR > maxRR) {
        maxRR = RR;
        maxRRID = p->id;
      }
      p = p->next;
    }

    p = RQs[queueIndex]->head;  // Remove process with highest RR from target Ready Queue
    Process *prev = nullptr;
    while (p) {
      if (p->id == maxRRID) {
        if (prev) {
          prev->next = p->next;
        }
        else {
          RQs[queueIndex]->head = p->next;
        }
        if (p == RQs[queueIndex]->tail) {
          RQs[queueIndex]->tail = prev;
        }
        RQs[queueIndex]->size--;
        break;
      }
      prev = p;
      p = p->next;
    }
    return p;
  }
  else {                        // Not HRRN, dequeue next process from target Ready Queue
    Process *p = RQs[queueIndex]->head;
    RQs[queueIndex]->head = RQs[queueIndex]->head->next;
    RQs[queueIndex]->size--;
    return p;
  }
}
