#ifndef TERMINAL_OUTPUT_H
#define TERMINAL_OUTPUT_H

#include <string>
#include <vector>

using namespace std;

const int DEFAULT_LINE_LENGTH = 50;

class TerminalOutput {
  private:
    static int lineLength;
  
    struct CenterLine {
      string line;
      int spaces;
    };

    static void printLine(char = '=', int = lineLength, bool = true);
    static void printMessageCentered(vector<CenterLine>);
    static vector<CenterLine> spliceMessage(string, int = lineLength);

  public:
    static void setLineLength(int length) {
      lineLength = length;
    }
    static void printTitle();
    static void printHeader(string);
};

#endif // TERMINAL_OUTPUT_H