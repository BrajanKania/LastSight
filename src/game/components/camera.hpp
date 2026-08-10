#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Camera {
    PROPERTY(Name = "Orthographic size")
    float orthographicSize{ 10.f };

    PROPERTY(Name = "Zoom")
    float zoom{ 1.f };

    PROPERTY(ReadOnly)
    glm::mat4 view{ 1.f };

    PROPERTY(ReadOnly)
    glm::mat4 projection{ 1.f };
  };

}  // namespace ls::component
