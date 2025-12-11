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

void TerminalOutput::printMessageCentered(vector<CenterLine> lines) {
  for (CenterLine line : lines) {
    printLine(' ', line.spaces, false);
    cout << line.line;
    printLine(' ', line.spaces);
  }
}

vector<TerminalOutput::CenterLine> TerminalOutput::spliceMessage(string message, int lineLength) {
  vector<CenterLine> lines;
  if (message.length() > lineLength) {
    // Split the message into multiple lines
    int targetLineLength = lineLength - 10;
    while (message.length() > targetLineLength) {
      // Find the first space in the message that is less than or equal to the target line length
      int count = -1;
      int offset = 0;
      do {
        count = message.find(' ', targetLineLength - offset);
        offset++;
      } while (count == -1);
      
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
  printLine();
  printMessageCentered(spliceMessage("DISCRETE TIME EVENT SIMULATOR"));
  printMessageCentered(spliceMessage("Heston Montagne"));
  printLine();
}

void TerminalOutput::printHeader(string message) {
  printLine();
  printMessageCentered(spliceMessage(message));
  printLine();
}