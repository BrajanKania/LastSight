#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct PostProcessSettings {
    glm::vec3 damageVignetteColor{ 1.f, 0.0f, 0.0f };
    float damageInnerRadius{ 0.2f };
    float damageOuterRadius{ 0.6f };
    float maxDamageDesaturation{ 0.5f };

    float staminaInnerRadius{ 0.2f };
    float staminaOuterRadius{ 0.6f };
    float maxStaminaDesaturation{ 1.0f };

    float currentDamageIntensity{ 0.f };
    float currentStaminaIntensity{ 0.f };
  };

}  // namespace ls::component
