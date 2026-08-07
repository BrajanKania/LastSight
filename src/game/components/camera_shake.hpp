#pragma once

#include <glm/ext/vector_float2.hpp>

namespace ls::component {

  struct CameraShake {
    glm::vec2 offset{ 0.f };
    glm::vec2 velocity{ 0.f };
    float stiffness{ 10.f };
    float damping{ 10.f };
    float maxSpringOffset{ 1.f };

    float trauma{ 0.f };
    float traumaDecay{ 2.f };
    float maxTraumaOffset{ 1.f };
    float traumaFrequency{ 10.f };
    float timeAccumulator{ 0.f };
  };

}  // namespace ls::component
