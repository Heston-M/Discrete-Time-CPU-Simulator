#ifndef CONFIG_H
#define CONFIG_H

#include "endChecker/endChecker.h"

// ====================================================================
// PROGRAM CONFIG VARIABLES
// ====================================================================

const bool PRINT_LIVE_UPDATES = false;  // Print live event updates to console (slows performance)

const int N = 10000;                    // Simulation halts after N processes depart

const bool CHOOSESCHEDULER = true;     // Prompt user to choose scheduler at runtime
const int DEFAULTSCHEDULER = 0;         // Default scheduler if not choosing at runtime (0 = FCFS, 1 = SJF)

const bool CHOOSENUMCPUS = true;        // Prompt user to choose number of CPUs at runtime
const int DEFAULTNUMCPUS = 1;           // Default number of CPUs if not choosing at runtime

const bool CHOOSERQSETUP = true;        // Prompt user to choose Ready Queue setup at runtime
const int DEFAULTRQSETUP = 2;           // Default Ready Queue setup if not choosing at runtime (2 = single global RQ)

const bool CHOOSEENDCONDITION = true;  // Prompt user to choose end condition at runtime
const EndCondition DEFAULTENDCONDITION = PROCESSES_DEPARTED;  // Default end condition if not choosing at runtime (0 = processes arrived, 1 = processes departed, 2 = time limit)
const int DEFAULTN = 10000;            // Default number of processes if not choosing at runtime
const float DEFAULTTIMELIMIT = 1000;   // Default time limit if not choosing at runtime

#endif // CONFIG_H

