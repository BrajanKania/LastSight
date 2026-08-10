#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Transform {
    PROPERTY(Name = "Scale")
    glm::vec2 scale{ 1.f };

    PROPERTY(Name = "Position")
    glm::vec2 position{ 0.f };

    PROPERTY(Name = "Rotation")
    float rotation{ 0.f };
  };

}  // namespace ls::component
