#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Stamina {
    float max{ 100.f };
    float current{ 100.f };
    float regenRate{ 20.f };
    float sprintCostRate{ 10.f };
    float aimCostRate{ 5.f };
    float recoveryThresholdRatio{ 0.5f };
    bool isExhausted{ false };
  };

}  // namespace ls::component
