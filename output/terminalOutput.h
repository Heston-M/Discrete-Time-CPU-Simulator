#ifndef TERMINAL_OUTPUT_H
#define TERMINAL_OUTPUT_H

#include <string>
#include <vector>

using namespace std;

const int LINE_LENGTH = 50;

class TerminalOutput {
  private:
    struct CenterLine {
      string line;
      int spaces;
    };

    static void printLine(char ch = '=', int length = LINE_LENGTH, bool newLine = true);
    static void printMessageCentered(vector<CenterLine> lines);
    static vector<CenterLine> spliceMessage(string message, int lineLength = LINE_LENGTH);

  public:
    static void printTitle();
    static void printHeader(string message);
};

#endif // TERMINAL_OUTPUT_H