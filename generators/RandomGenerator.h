#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

// ====================================================================
// Structure to generate uniformly distributed random integers
class RandomGenerator {
  public:
    RandomGenerator();

    // Generate a uniformly-random int in the range [0, maxIndex)
    int getRandomIndex(int);
    float getRandomFloat(float = 0.0, float = 1.0);
};

#endif // RANDOMGENERATOR_H