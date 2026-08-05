#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

namespace ls::particle {

  struct ParticleEmitterConfig {
    std::size_t maxParticles{ 10 };

    float minSpeed{ 0.f };
    float maxSpeed{ 1.f };

    glm::vec2 minScale{ 0.5f };
    glm::vec2 maxScale{ 1.f };
    glm::vec2 endScale{ 0.f };

    float minRotation{ -180.f };
    float maxRotation{ 180.f };

    float minAngularSpeed{ 0.f };
    float maxAngularSpeed{ 90.f };

    glm::vec4 startColor{ 1.f };
    glm::vec4 endColor{ 0.f };

    float minBrightness{ 0.7f };
    float maxBrightness{ 1.3f };

    float minLifetime{ 0.1f };
    float maxLifetime{ 1.f };

    bool isLooping{ false };
    float spawnRate{ 1.f };

    float duration{ 10.f };
  };

}  // namespace ls::particle
