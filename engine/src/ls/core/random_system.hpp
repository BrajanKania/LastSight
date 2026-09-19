#pragma once

#include <cstdint>

namespace ls::random_system {

  float getRangeRandom(float min, float max);

  int getRangeRandom(int min, int max);

  std::uint64_t getRandom64();

}  // namespace ls::random_system
