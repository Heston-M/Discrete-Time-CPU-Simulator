#include "InputHandler.h"
#include "../config.h"
#include <string>
#include <iostream>
#include <limits>

using namespace std;

template <typename Type>

Type InputHandler::performInputLoop(InputType inputType) {
  Type input;
  bool validInput = false;
  string message = "";

  switch (inputType) {
    case ARRIVAL_RATE:
      message = "Enter the average arrival rate (processes per second): ";
      break;
    case SERVICE_TIME:
      message = "Enter the average service time (seconds): ";
      break;
    case SCHEDULER:
      message = "Pick the scheduler (0 for FCFS, 1 for SJF): ";
      break;
    case RQ_SETUP:
      message = "Pick the Ready Queue setup (1 for RQ per-CPU, 2 for single global RQ): ";
      break;
    case NUM_CPUS:
      message = "Enter the number of CPUs: ";
      break;
    case END_CONDITION:
      message = "Pick the end condition (0 for processes arrived, 1 for processes departed, 2 for time limit): ";
      break;
    case N:
      message = "Enter the number of processes: ";
      break;
    case TIME_LIMIT:
      message = "Enter the time limit (seconds): ";
      break;
    default:
      throw runtime_error("Invalid input type.");
  }

  while (!validInput) {
    cout << message;
    cin >> input;
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    switch (inputType) {
      case ARRIVAL_RATE:
        if (input > 0) {
          validInput = true;
        }
        break;
      case SERVICE_TIME:
        if (input > 0) {
          validInput = true;
        }
        break;
      case SCHEDULER:
        if (input == 0 || input == 1) {
          validInput = true;
        }
        break;
      case RQ_SETUP:
        if (input == 1 || input == 2) {
          validInput = true;
        }
        break;
      case NUM_CPUS:
        if (input > 0) {
          validInput = true;
        }
        break;
      case END_CONDITION:
        if (input == 0 || input == 1 || input == 2) {
          validInput = true;
        }
        break;
      case N:
        if (input > 0) {
          validInput = true;
        }
        break;
      case TIME_LIMIT:
        if (input > 0) {
          validInput = true;
        }
        break;
      default:
        throw runtime_error("Invalid input type.");
    }
    if (!validInput) {
      cout << "Invalid input. Please try again." << endl;
    }
  }
  cout << endl;
  return input;
}

float InputHandler::getArrivalLambda() {
  return performInputLoop<float>(ARRIVAL_RATE);
}

float InputHandler::getServiceTimeAvg() {
  return performInputLoop<float>(SERVICE_TIME);
}

int InputHandler::getSchedulerType() {
  if (CHOOSESCHEDULER) {
    return performInputLoop<int>(SCHEDULER);
  }
  else {
    return DEFAULTSCHEDULER;
  }
}

int InputHandler::getRQSetup() {
  if (CHOOSERQSETUP) {
    return performInputLoop<int>(RQ_SETUP);
  }
  else {
    return DEFAULTRQSETUP;
  }
}

int InputHandler::getNumCPUs() {
  if (CHOOSENUMCPUS) {
    return performInputLoop<int>(NUM_CPUS);
  }
  else {
    return DEFAULTNUMCPUS;
  }
}

EndCondition InputHandler::getEndCondition() {
  if (CHOOSEENDCONDITION) {
    return static_cast<EndCondition>(performInputLoop<int>(END_CONDITION));
  }
  else {
    return DEFAULTENDCONDITION;
  }
}

int InputHandler::getN() {
  if (CHOOSEENDCONDITION) {
    return performInputLoop<int>(N);
  }
  else {
    return DEFAULTN;
  }
}

float InputHandler::getTimeLimit() {
  if (CHOOSEENDCONDITION) {
    return performInputLoop<float>(TIME_LIMIT);
  }
  else {
    return DEFAULTTIMELIMIT;
  }
}