#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

class InputHandler {

  private:
    enum InputType {
      ARRIVAL_RATE,
      SERVICE_TIME,
      SCHEDULER,
      RQ_SETUP,
      NUM_CPUS
    };

    template <typename Type>
    Type performInputLoop(InputType);

    float arrivalLambda;
    float serviceTimeAvg;
    int schedulerType;
    int rqSetup;
    int numCPUs;

  public:
    void handleInput(bool, bool, bool);
    void handleInput();

    float getArrivalLambda();
    float getServiceTimeAvg();
    int getSchedulerType();
    int getRQSetup();
    int getNumCPUs();
};

#endif // INPUTHANDLER_H