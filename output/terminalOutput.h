#ifndef TERMINAL_OUTPUT_H
#define TERMINAL_OUTPUT_H

#include "output.h"

using namespace std;

class TerminalOutput : public Output {
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
    void printTitle();
    void printHeader(string);
    void printMetric(MetricType metricType, vector<float> values);
};

#endif // TERMINAL_OUTPUT_H