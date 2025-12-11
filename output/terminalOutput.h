#ifndef TERMINAL_OUTPUT_H
#define TERMINAL_OUTPUT_H

#include <string>

using namespace std;

const int LINE_LENGTH = 50;

class TerminalOutput {
  private:
      static void printLine(char ch = '=', int length = LINE_LENGTH, bool newLine = true);

  public:
    static void printTitle();
    static void printHeader(string message);
};

#endif // TERMINAL_OUTPUT_H