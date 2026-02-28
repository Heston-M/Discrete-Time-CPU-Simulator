#include "InputHandler.h"
#include "../config.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <type_traits>
#include <stdexcept>

using namespace std;

map<InputHandler::InputType, string> InputHandler::inputMessages = {
  {InputHandler::ARRIVAL_RATE, "Enter the average arrival rate (processes per second): "},
  {InputHandler::SERVICE_TIME, "Enter the average service time (seconds): "},
  {InputHandler::SCHEDULER, "Pick the scheduler (0 for FCFS, 1 for SJF, 2 for SRTF, 3 for HRRN): "},
  {InputHandler::RQ_SETUP, "Pick the Ready Queue setup (1 for RQ per-CPU, 2 for single global RQ): "},
  {InputHandler::NUM_CPUS, "Enter the number of CPUs: "},
  {InputHandler::END_CONDITION, "Pick the end condition (0 for processes arrived, 1 for processes departed, 2 for time limit): "},
  {InputHandler::N, "Enter the number of processes: "},
  {InputHandler::TIME_LIMIT, "Enter the time limit (seconds): "}
};

map<InputHandler::InputType, bool> InputHandler::inputChoices = {
  {InputHandler::ARRIVAL_RATE, true},
  {InputHandler::SERVICE_TIME, true},
  {InputHandler::SCHEDULER, CHOOSE_SCHEDULER},
  {InputHandler::RQ_SETUP, CHOOSE_RQ_SETUP},
  {InputHandler::NUM_CPUS, CHOOSE_NUM_CPUS},
  {InputHandler::END_CONDITION, CHOOSE_END_CONDITION},
  {InputHandler::N, CHOOSE_END_CONDITION},
  {InputHandler::TIME_LIMIT, CHOOSE_END_CONDITION}
};

map<InputHandler::InputType, float> InputHandler::floatDefaults = {
  {InputHandler::ARRIVAL_RATE, 1.0},
  {InputHandler::SERVICE_TIME, 1.0},
  {InputHandler::TIME_LIMIT, DEFAULT_TIME_LIMIT}
};

map<InputHandler::InputType, int> InputHandler::intDefaults = {
  {InputHandler::SCHEDULER, DEFAULT_SCHEDULER},
  {InputHandler::RQ_SETUP, DEFAULT_RQ_SETUP},
  {InputHandler::NUM_CPUS, DEFAULT_NUM_CPUS},
  {InputHandler::END_CONDITION, static_cast<int>(DEFAULT_END_CONDITION)},
  {InputHandler::N, DEFAULT_N}
};

template <typename Type>
Type InputHandler::getDefault(InputType inputType) {
  if (is_same<Type, float>::value) {
    return floatDefaults[inputType];
  }
  else if (is_same<Type, int>::value) {
    return intDefaults[inputType];
  }
  else {
    throw runtime_error("Invalid input type.");
  }
}

template <typename Type>
Type InputHandler::performInputLoop(InputType inputType) {
  Type input;
  bool validInput = false;
  string message = inputMessages[inputType];

  while (!validInput) {
    cout << message;
    string line;
    if (!getline(cin, line)) {
      if (cin.eof()) {
        throw runtime_error("End of input.");
      }
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      continue;
    }
    try {
      double floatInput = stof(line);
      if (is_same<Type, int>::value && floatInput != floor(floatInput)) { // Check if input is an integer when input type is int
        cout << "Input must be an integer. Please try again." << endl;
        continue;
      }
      input = static_cast<Type>(floatInput);
    }
    catch (const invalid_argument &e) {
      cout << "Input must be a number. Please try again." << endl;
      continue;
    }
    catch (const out_of_range &e) {
      cout << "Input is too large or too small. Please try again." << endl;
      continue;
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
        if (0 <= input && input <= 3) {
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
        if (0 <= input && input <= 2) {
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

template <typename Type>
Type InputHandler::getInput(InputType inputType) {
  if (inputChoices[inputType]) {
    return performInputLoop<Type>(inputType);
  }
  else {
    return getDefault<Type>(inputType);
  }
}

template float InputHandler::getInput<float>(InputType);
template int InputHandler::getInput<int>(InputType);