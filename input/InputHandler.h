#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../endChecker/EndCondition.h"
#include <string>
#include <map>

using namespace std;

class InputHandler {

  public:
    enum InputType {
      ARRIVAL_RATE,
      SERVICE_TIME,
      SCHEDULER,
      RQ_SETUP,
      NUM_CPUS,
      END_CONDITION,
      N,
      TIME_LIMIT,
      AFFINITY_PROBABILITY
    };

    template <typename Type>
    static Type getInput(InputType);

  private:
    static map<InputType, string> inputMessages;
    static map<InputType, bool> inputChoices;
    static map<InputType, float> floatDefaults;
    static map<InputType, int> intDefaults;

    template <typename Type>
    static Type getDefault(InputType);

    template <typename Type>
    static Type performInputLoop(InputType);
};

#endif // INPUTHANDLER_H