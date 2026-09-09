#pragma once

#include <glm/glm.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct PostProcessSettings {
    PROPERTY(Color)
    glm::vec3 damageVignetteColor{ 1.f, 0.0f, 0.0f };

    PROPERTY()
    float damageInnerRadius{ 0.2f };

    PROPERTY()
    float damageOuterRadius{ 0.6f };

    PROPERTY()
    float maxDamageDesaturation{ 0.5f };

    PROPERTY()
    float staminaInnerRadius{ 0.2f };

    PROPERTY()
    float staminaOuterRadius{ 0.6f };

    PROPERTY()
    float maxStaminaDesaturation{ 1.0f };

    PROPERTY(Transient)
    float currentDamageIntensity{ 0.f };

    PROPERTY(Transient)
    float currentStaminaIntensity{ 0.f };
  };

}  // namespace ls::component
