#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

// ====================================================================
// Structure to generate uniformly distributed random integers
class RandomGenerator {
  public:
    RandomGenerator();

    // Generate a uniformly-random int in the range [0, maxIndex)
    int getRandomIndex(int);
};

#endif // RANDOMGENERATOR_H