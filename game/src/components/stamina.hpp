#pragma once

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Stamina {
    PROPERTY()
    float max{ 100.f };

    PROPERTY()
    float current{ 100.f };

    PROPERTY()
    float regenRate{ 20.f };

    PROPERTY()
    float sprintCostRate{ 10.f };

    PROPERTY()
    float aimCostRate{ 5.f };

    PROPERTY()
    float recoveryThresholdRatio{ 0.5f };

    PROPERTY()
    bool isExhausted{ false };
  };

}  // namespace ls::component
