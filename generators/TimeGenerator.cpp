#include "TimeGenerator.h"
#include <iostream>
#include <ctime>
#include <cmath>

using namespace std;

float TimeGenerator::arrivalLambda = 0.0;
float TimeGenerator::serviceTimeAvg = 0.0;

TimeGenerator::TimeGenerator(float aLamb, float sTimeAvg) {
  TimeGenerator::arrivalLambda = aLamb;
  TimeGenerator::serviceTimeAvg = sTimeAvg;

  // Seed random number generator
  srand(time(0));
}

float TimeGenerator::getInterArrivalTime() {
  if (!TimeGenerator::arrivalLambda) {
    throw runtime_error("Error: Average Arrival Rate not set.");
  }
  float p = double(rand() + 1.0) / double(RAND_MAX + 1.0);
  float x = -log(p) / TimeGenerator::arrivalLambda;

  if (isinf(x)) throw runtime_error("Error: interarrival time is infinite.");
  if (isnan(x)) throw runtime_error("Error: interarrival time is NaN.");

  return x;
}

float TimeGenerator::getServiceTime() {
  if (!TimeGenerator::serviceTimeAvg) {
    throw runtime_error("Error: Average Service Time not set.");
  }
  float p = ((double)rand() + 1.0) / double(RAND_MAX + 1.0);
  float x = -log(p) * TimeGenerator::serviceTimeAvg;

  if (isinf(x)) throw runtime_error("Error: service time is infinite.");
  if (isnan(x)) throw runtime_error("Error: service time is NaN.");

  return x;
}
