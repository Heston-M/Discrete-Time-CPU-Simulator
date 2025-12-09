#include "endChecker.h"
#include "../input/InputHandler.h"

EndChecker::EndChecker() {
  this->endCondition = InputHandler::getEndCondition();
  if (endCondition == TIME_LIMIT) {
    this->timeLimit = InputHandler::getTimeLimit();
    this->N = -1;
  }
  else {
    this->N = InputHandler::getN();
    this->timeLimit = -1;
  }
  this->arrivals = 0;
  this->departures = 0;
  this->clock = 0;
  this->endConditionMet = false;
}

bool EndChecker::checkEnd() {
  return endConditionMet;
}

void EndChecker::logArrival(float clock) {
  arrivals++;
  this->clock = clock;

  if (endCondition == PROCESSES_ARRIVED && arrivals >= N) {
    endConditionMet = true;
  }
  else if (endCondition == TIME_LIMIT && clock >= timeLimit) {
    endConditionMet = true;
  }
}

void EndChecker::logDeparture(float clock) {
  departures++;
  this->clock = clock;

  if (endCondition == PROCESSES_DEPARTED && departures >= N) {
    endConditionMet = true;
  }
  else if (endCondition == TIME_LIMIT && clock >= timeLimit) {
    endConditionMet = true;
  }
}