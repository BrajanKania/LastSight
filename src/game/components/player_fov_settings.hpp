#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct PlayerFovSettings {
    PROPERTY()
    float baseAngle{ 100.f };

    PROPERTY()
    float aimAngle{ 20.f };

    PROPERTY()
    float lowStaminaAngle{ 50.f };

    PROPERTY()
    float baseSmoothnessAngle{ 25.f };

    PROPERTY()
    float aimSmoothnessAngle{ 5.f };

    PROPERTY()
    float lowStaminaSmoothnessAngle{ 30.f };

    PROPERTY()
    float baseDarkness{ 0.6f };

    PROPERTY()
    float aimDarkness{ 0.3f };

    PROPERTY()
    float lowStaminaThreshold{ 0.5f };

    PROPERTY()
    float transitionSpeed{ 10.f };
  };

}  // namespace ls::component
