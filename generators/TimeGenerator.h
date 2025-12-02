#ifndef TIMEGENERATOR_H
#define TIMEGENERATOR_H

// ====================================================================
// Structure to generate exponentially distributed times
// Used for service times and inter-arrival times
class TimeGenerator {

  private:
    static float arrivalLambda;
    static float serviceTimeAvg;

  public:
    TimeGenerator(float, float);

    float getInterArrivalTime();
    float getServiceTime();
};

#endif // TIMEGENERATOR_H