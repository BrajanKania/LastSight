#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::particle {

  STRUCT()
  struct Particle {
    glm::vec2 position{ 0.f };
    glm::vec2 velocity{ 0.f };

    glm::vec2 startScale{ 1.f };
    glm::vec2 endScale{ 0.5f };
    glm::vec2 scale{ 1.f };

    float rotation{ 0.f };
    float angularSpeed{ 0.f };

    glm::vec4 startColor{ 1.f };
    glm::vec4 endColor{ 0.f };
    glm::vec4 color{ 1.f };

    float lifetime{ 0.f };
    float maxLifetime{ 1.f };
  };

}  // namespace ls::particle
