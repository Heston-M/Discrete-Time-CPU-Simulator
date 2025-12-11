#ifndef CONFIG_H
#define CONFIG_H

#include "endChecker/EndCondition.h"

// ====================================================================
// PROGRAM CONFIG VARIABLES
// ====================================================================

const bool PRINT_LIVE_UPDATES = false;  // Print live event updates to console (slows performance)

const bool CHOOSE_SCHEDULER = true;      // Prompt user to choose scheduler at runtime
const bool CHOOSE_NUM_CPUS = true;        // Prompt user to choose number of CPUs at runtime
const bool CHOOSE_RQ_SETUP = true;        // Prompt user to choose Ready Queue setup at runtime
const bool CHOOSE_END_CONDITION = true;   // Prompt user to choose end condition at runtime

const int DEFAULT_SCHEDULER = 0;         // Default scheduler if not choosing at runtime (0 = FCFS, 1 = SJF)
const int DEFAULT_NUM_CPUS = 1;           // Default number of CPUs if not choosing at runtime
const int DEFAULT_RQ_SETUP = 2;           // Default Ready Queue setup if not choosing at runtime (2 = single global RQ)

const EndCondition DEFAULT_END_CONDITION = PROCESSES_DEPARTED;  // Default end condition if not choosing at runtime (0 = processes arrived, 1 = processes departed, 2 = time limit)
const int DEFAULT_N = 10000;             // Default number of processes if not choosing at runtime
const float DEFAULT_TIME_LIMIT = 100;     // Default time limit (in seconds) if not choosing at runtime

#endif // CONFIG_H

