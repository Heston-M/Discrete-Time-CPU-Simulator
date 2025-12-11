#include "terminalOutput.h"
#include <iostream>

using namespace std;

void TerminalOutput::printLine(char ch, int length, bool newLine) {
  for (int i = 0; i < length; i++) {
    cout << ch;
  }
  if (newLine) {
    cout << endl;
  }
}

void TerminalOutput::printTitle() {
  printLine();
  cout << "          DISCRETE TIME EVENT SIMULATOR           " << endl;
  cout << "               - Heston Montagne -                " << endl;
  printLine();
}

void TerminalOutput::printHeader(string message) {
  printLine();
  printLine(' ', LINE_LENGTH - message.length(), false);
  cout << message;
  printLine(' ', LINE_LENGTH - message.length());
  printLine();
}