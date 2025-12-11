#ifndef TERMINAL_OUTPUT_H
#define TERMINAL_OUTPUT_H

#include <string>
#include <vector>

using namespace std;

class TerminalOutput {
  private:
    static int lineLength;
    static int padding;
  
    struct CenterLine {
      string line;
      int spaces;
    };

    static void printLine(char = '=', int = lineLength, bool = true);
    static void printMessageCentered(vector<CenterLine>);
    static vector<CenterLine> spliceMessage(string, int = lineLength);

  public:
    static void setLineLength(int newLength) {
      lineLength = newLength;
    }
    static void setPadding(int newPadding) {
      padding = newPadding;
    }
    static void printTitle();
    static void printHeader(string);
};

#endif // TERMINAL_OUTPUT_H