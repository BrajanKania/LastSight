#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct FieldOfView {
    PROPERTY()
    float innerRadius{ 1.f };

    PROPERTY()
    float outerRadius{ 10.f };

    PROPERTY()
    float fovAngle{ 60.f };

    PROPERTY()
    float smoothnessAngle{ 4.f };

    PROPERTY()
    float smoothnessDistance{ 1.f };

    PROPERTY()
    float darkness{ 1.f };
  };

}  // namespace ls::component
