#ifndef ENDCHECKER_H
#define ENDCHECKER_H

#include "EndCondition.h"

class EndChecker {
  private:
    EndCondition endCondition;
    int N;
    float timeLimit;

    int arrivals;
    int departures;
    float clock;
    
    bool endConditionMet;

  public:
    EndChecker();

    bool checkEnd();

    void logArrival(float);
    void logDeparture(float);
};

#endif // ENDCHECKER_H