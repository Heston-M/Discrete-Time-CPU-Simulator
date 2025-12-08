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
    }
    if (!validInput) {
      cout << "Invalid input. Please try again." << endl;
    }
  }
  cout << endl;
  return input;
}

void InputHandler::handleInput(bool chooseScheduler, bool chooseNumCPUs, bool chooseRQSetup) {
  
  // Arrival rate
  this->arrivalLambda = performInputLoop<float>(ARRIVAL_RATE);

  // Service time
  this->serviceTimeAvg = performInputLoop<float>(SERVICE_TIME);

  // Scheduler
  if (chooseScheduler) {
    this->schedulerType = performInputLoop<int>(SCHEDULER);
  }
  else {
    this->schedulerType = DEFAULTSCHEDULER;
  }

  // Ready Queue setup
  if (chooseRQSetup) {
    this->rqSetup = performInputLoop<int>(RQ_SETUP);
  }
  else {
    this->rqSetup = DEFAULTRQSETUP;
  }

  // Number of CPUs
  if (chooseNumCPUs) {
    this->numCPUs = performInputLoop<int>(NUM_CPUS);
  }
  else {
    this->numCPUs = DEFAULTNUMCPUS;
  }
}

void InputHandler::handleInput() {
  handleInput(CHOOSESCHEDULER, CHOOSENUMCPUS, CHOOSERQSETUP);
}

float InputHandler::getArrivalLambda() {
  return this->arrivalLambda;
}

float InputHandler::getServiceTimeAvg() {
  return this->serviceTimeAvg;
}

int InputHandler::getSchedulerType() {
  return this->schedulerType;
}

int InputHandler::getRQSetup() {
  return this->rqSetup;
}

int InputHandler::getNumCPUs() {
  return this->numCPUs;
}