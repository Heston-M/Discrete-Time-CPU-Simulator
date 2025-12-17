#include "../../output/output.h"
#include "../../output/terminalOutput.h"
#include "../../processes/Process.h"
#include "../../processes/ReadyQueueList.h"

int main() {
  Output* out = new TerminalOutput();

  out->printTitle();
  out->printHeader("Hello, world!");
  out->printHeader("This is a test message.");
  out->printHeader("This is a test message with a long message.");
  out->printHeader("This is a test message with a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message and a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message and a long message and a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message and a long message and a long message and a long message and a long message.");
  out->printHeader("This is a test message with a long message and a long message and a long message and a long message and a long message and a long message and a long message and a long message.");
  
  out->printMetric(Output::AVG_TURN_TIME, {10.0});
  out->printMetric(Output::TOTAL_THROUGHPUT, {100.0});
  out->printMetric(Output::CPU_UTILIZATION, {0.5});
  out->printMetric(Output::AVG_PROCESSES_IN_Q, {10.0});

  Process* process = new Process(10.0, 10.0);
  ReadyQueueList* RQList = new ReadyQueueList(1, 0);
  
  out->printLiveUpdate(10.0, Output::ARRIVAL_TO_CPU, process, RQList, nullptr);
  out->printLiveUpdate(10.0, Output::ARRIVAL_TO_RQ, process, RQList, nullptr);
  out->printLiveUpdate(10.0, Output::DEPARTURE_CPU_IDLE, process, RQList, nullptr);
  out->printLiveUpdate(10.0, Output::DEPARTURE_NEXT_PROCESS, process, RQList, process);
  
  delete process;
  delete RQList;
  delete out;

  return 0;
}