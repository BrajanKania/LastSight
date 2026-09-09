#pragma once

#include <glm/glm.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Camera {
    PROPERTY(DisplayName("Orthographic size"))
    float orthographicSize{ 10.f };

    PROPERTY(DisplayName("Zoom"))
    float zoom{ 1.f };

    PROPERTY(Transient)
    glm::mat4 view{ 1.f };

    PROPERTY(Transient)
    glm::mat4 projection{ 1.f };
  };

}  // namespace ls::component
