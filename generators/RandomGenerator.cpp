#include "RandomGenerator.h"
#include <cstdlib>
#include <ctime>

RandomGenerator::RandomGenerator() {
  srand(time(0));
}

// Generate a uniformly-random int in the range [0, maxIndex)
int RandomGenerator::getRandomIndex(int maxIndex) {
  return (double(rand())) / (double(RAND_MAX + 1.0)) * maxIndex;
}