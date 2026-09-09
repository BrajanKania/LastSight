#include "ls/core/random_system.hpp"

#include <cassert>
#include <random>

namespace ls::random_system {

  float getRangeRandom(float min, float max) {
    static std::mt19937 gen{ std::random_device{}() };
    assert(min < max);
    std::uniform_real_distribution<float> dist{ min, max };
    return dist(gen);
  }

  int getRangeRandom(int min, int max) {
    static std::mt19937 gen{ std::random_device{}() };
    assert(min < max);
    std::uniform_int_distribution<int> dist{ min, max };
    return dist(gen);
  }

}  // namespace ls::random_system
