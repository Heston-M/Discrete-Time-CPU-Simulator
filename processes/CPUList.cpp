#include "CPUList.h"

CPUList::CPUList(int numCPUs) {
  for (int i = 0; i < numCPUs; i++) {
    CPUs.push_back(nullptr);
  }
}

CPUList::~CPUList() {
  for (int i = 0; i < CPUs.size(); i++) {
    if (CPUs[i]) {
      delete CPUs[i];
    }
  }
}

int CPUList::getNumCPUs() {
  return CPUs.size();
}

bool CPUList::isCPUIdle(int cpuIndex = 0) {
  return CPUs[cpuIndex] == nullptr;
}

vector<int> CPUList::getIdleCPUs() {
  vector<int> idleCPUs;
  for (int i = 0; i < CPUs.size(); i++) {
    if (CPUs[i] == nullptr) {
      idleCPUs.push_back(i);
    }
  }
  return idleCPUs;
}

void CPUList::assignProcessToCPU(Process *process, int cpuIndex = 0) {
  CPUs[cpuIndex] = process;
  if (process) process->CPUindex = cpuIndex;
}

Process* CPUList::getProcessOnCPU(int cpuIndex = 0) {
  return CPUs[cpuIndex];
}

Process* CPUList::removeProcessFromCPU(int cpuIndex = 0) {
  Process *p = CPUs[cpuIndex];
  CPUs[cpuIndex] = nullptr;
  return p;
}