#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../endChecker/EndCondition.h"

class InputHandler {

  private:
    enum InputType {
      ARRIVAL_RATE,
      SERVICE_TIME,
      SCHEDULER,
      RQ_SETUP,
      NUM_CPUS,
      END_CONDITION,
      N,
      TIME_LIMIT
    };

    template <typename Type>
    Type performInputLoop(InputType);

    float arrivalLambda;
    float serviceTimeAvg;
    int schedulerType;
    int rqSetup;
    int numCPUs;

  public:
    float getArrivalLambda();
    float getServiceTimeAvg();
    int getSchedulerType();
    int getRQSetup();
    int getNumCPUs();

    EndCondition getEndCondition();
    int getN();
    float getTimeLimit();
};

#endif // INPUTHANDLER_H