#pragma once

#include <glm/glm.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Transform {
    PROPERTY(DisplayName("Scale"), Step(0.01f))
    glm::vec2 scale{ 1.f };

    PROPERTY(DisplayName("Position"), Step(0.01f))
    glm::vec2 position{ 0.f };

    PROPERTY(DisplayName("Rotation"), Step(0.5f), Range(-180.f, 180.f))
    float rotation{ 0.f };
  };

}  // namespace ls::component
