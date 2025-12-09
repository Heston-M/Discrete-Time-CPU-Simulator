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
    static Type performInputLoop(InputType);

  public:
    static float getArrivalLambda();
    static float getServiceTimeAvg();
    static int getSchedulerType();
    static int getRQSetup();
    static int getNumCPUs();

    static EndCondition getEndCondition();
    static int getN();
    static float getTimeLimit();
};

#endif // INPUTHANDLER_H