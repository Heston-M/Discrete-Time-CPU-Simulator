#include "endChecker.h"
#include "../input/InputHandler.h"

EndChecker::EndChecker() {
  this->endCondition = static_cast<EndCondition>(InputHandler::getInput<int>(InputHandler::END_CONDITION));
  if (endCondition == TIME_LIMIT) {
    this->timeLimit = InputHandler::getInput<float>(InputHandler::TIME_LIMIT);
    this->N = -1;
  }
  else {
    this->N = InputHandler::getInput<int>(InputHandler::N);
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