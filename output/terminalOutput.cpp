#include "terminalOutput.h"
#include "../config.h"
#include <iostream>
#include <iomanip>
#include <unordered_map>

unordered_map<Output::MetricType, string> metricTypeMap = {
  {Output::AVG_TURN_TIME, "Average Turnaround Time"},
  {Output::TOTAL_THROUGHPUT, "Total Throughput"},
  {Output::CPU_UTILIZATION, "CPU Utilization"},
  {Output::AVG_PROCESSES_IN_Q, "Average Number of Processes in the Ready Queue"},
};

unordered_map<Output::MetricType, string> metricUnitMap = {
  {Output::AVG_TURN_TIME, "seconds"},
  {Output::TOTAL_THROUGHPUT, "processes per second"},
  {Output::CPU_UTILIZATION, ""},
  {Output::AVG_PROCESSES_IN_Q, "processes"},
};

using namespace std;

int TerminalOutput::lineLength = DEFAULT_LINE_LENGTH;
int TerminalOutput::padding = DEFAULT_PADDING;

void TerminalOutput::printLine(char ch, int length, bool newLine) {
  for (int i = 0; i < length; i++) {
    cout << ch;
  }
  if (newLine) {
    cout << endl;
  }
}

void TerminalOutput::printMessageCentered(vector<CenterLine> lines) {
  for (CenterLine line : lines) {
    printLine(' ', line.spaces, false);
    cout << line.line;
    printLine(' ', line.spaces);
  }
}

vector<TerminalOutput::CenterLine> TerminalOutput::spliceMessage(string message, int lineLength) {
  vector<CenterLine> lines;
  int targetLineLength = lineLength - padding;
  if (message.length() > targetLineLength) {
    // Split the message into multiple lines
    while (message.length() > targetLineLength) {
      // Find the first space in the message that is less than or equal to the target line length
      int count = -1;
      int offset = 0;
      do {
        count = message.find(' ', targetLineLength - offset);
        offset++;
      } while (count == -1 || count > targetLineLength);
      
      // Add the line to the vector
      string line = message.substr(0, count);
      lines.push_back({line, int((lineLength - line.length()) / 2)});

      // Remove the line from the message
      message = message.substr(count);
    }
  }
  // Add the last (or only) line to the vector
  lines.push_back({message, int((lineLength - message.length()) / 2)});
  return lines;
}

void TerminalOutput::printTitle() {
  cout << endl;
  printLine();
  printMessageCentered(spliceMessage("DISCRETE TIME EVENT SIMULATOR"));
  printMessageCentered(spliceMessage("Heston Montagne"));
  printLine();
  cout << endl;
}

void TerminalOutput::printHeader(string message) {
  printLine();
  printMessageCentered(spliceMessage(message));
  printLine();
  cout << endl;
}

void TerminalOutput::printMetric(MetricType metricType, vector<float> values) {
  cout << setprecision(4) << metricTypeMap[metricType] << ": ";
  if (values.size() == 1) {
    cout << values[0] << " " << metricUnitMap[metricType] << endl;
  } else {
    cout << endl;
    for (int i = 0; i < values.size(); i++) {
      cout << "    CPU " << i << ": " << values[i] << " " << metricUnitMap[metricType] << endl;
    }
  }
  cout << endl;
}

void TerminalOutput::printLiveUpdate(float clock, LiveUpdateType eventType, Process *process, ReadyQueueList *RQList, Process *nextProcess) {
  cout << fixed << setprecision(4) << clock << " | ";

  switch (eventType) {
    case ARRIVAL_TO_CPU:
      cout << "Process " << process->id << " arrived. CPU " << process->CPUindex 
           << " was idle, so process " << process->id << " (" << process->serviceTime 
           << ") started running on CPU " << process->CPUindex << ". ";
      break;
    case ARRIVAL_TO_RQ:
      cout << "Process " << process->id << " arrived. ";
      if (RQList->getNumRQs() == 1) {
        cout << "No CPU was idle, so the process was added to Ready Queue (" << RQList->getRQSize(0) << "). ";
      } else {
        cout << "CPU " << process->RQindex << " was busy, so the process was added to Ready Queue " 
             << process->RQindex << " (" << RQList->getRQSize(process->RQindex) << "). ";
      }
      break;
    case DEPARTURE_CPU_IDLE:
      cout << "Process " << process->id << " departed from CPU " << process->CPUindex << ". ";
      cout << "CPU " << process->CPUindex << " is now idle. ";
      break;
    case DEPARTURE_NEXT_PROCESS:
      cout << "Process " << process->id << " departed from CPU " << process->CPUindex << ". ";
      cout << "Process " << nextProcess->id << " (" << nextProcess->serviceTime << ") moving to CPU " 
           << process->CPUindex << ". ";
      break;
    case PREEMPTION_INTERVAL:
      cout << "Process " << process->id << " was preempted after running for " << clock - process->lastRunTime 
           << " seconds on CPU " << process->CPUindex << ". ";
      if (process->id == nextProcess->id) {
        cout << "The Ready Queue was empty, so the process was restarted on CPU " << process->CPUindex << ". ";
      } else {
        cout << "The process was added to Ready Queue " << process->RQindex << " (" 
             << RQList->getRQSize(process->RQindex) << ") and process " << nextProcess->id 
             << " (" << nextProcess->serviceTime << ") started running on CPU " << nextProcess->CPUindex << ". ";
      }
  }

  cout << endl;
}