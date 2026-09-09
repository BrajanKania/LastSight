#pragma once

#include <glm/ext/vector_float2.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct CameraShake {
    PROPERTY(ReadOnly)
    glm::vec2 offset{ 0.f };

    PROPERTY(ReadOnly)
    glm::vec2 velocity{ 0.f };

    PROPERTY()
    float stiffness{ 10.f };

    PROPERTY()
    float damping{ 10.f };

    PROPERTY()
    float maxSpringOffset{ 1.f };

    PROPERTY()
    float trauma{ 0.f };

    PROPERTY(Name = "Trauma decay")
    float traumaDecay{ 2.f };

    PROPERTY()
    float maxTraumaOffset{ 1.f };

    PROPERTY()
    float traumaFrequency{ 10.f };

    PROPERTY(ReadOnly)
    float timeAccumulator{ 0.f };
  };

}  // namespace ls::component
