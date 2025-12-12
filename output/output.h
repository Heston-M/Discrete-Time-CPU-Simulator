#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

class Output {
  public:
    virtual void printTitle() = 0;
    virtual void printHeader(std::string message) = 0;

    virtual ~Output() = default;
};

#endif // OUTPUT_H