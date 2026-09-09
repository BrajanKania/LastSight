#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

#include "ls/gfx/texture_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/layer.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::particle {

  STRUCT()
  struct ParticleEmitterConfig {
    PROPERTY()
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY()
    gfx::TextureHandle textureHandle{ 0 };

    PROPERTY()
    renderer::Layer layer{ renderer::Layer::Entities };

    PROPERTY(DisplayName("Max particles"), ReadOnly)
    std::size_t maxParticles{ 10 };

    PROPERTY()
    float minSpeed{ 0.f };

    PROPERTY()
    float maxSpeed{ 1.f };

    PROPERTY()
    glm::vec2 minScale{ 0.5f };

    PROPERTY()
    glm::vec2 maxScale{ 1.f };

    PROPERTY()
    glm::vec2 endScale{ 0.f };

    PROPERTY()
    float minRotation{ -180.f };

    PROPERTY()
    float maxRotation{ 180.f };

    PROPERTY()
    float minAngularSpeed{ 0.f };

    PROPERTY()
    float maxAngularSpeed{ 90.f };

    PROPERTY(color)
    glm::vec4 startColor{ 1.f };

    PROPERTY(color)
    glm::vec4 endColor{ 0.f };

    PROPERTY()
    float minBrightness{ 0.7f };

    PROPERTY()
    float maxBrightness{ 1.3f };

    PROPERTY()
    float minLifetime{ 0.1f };

    PROPERTY()
    float maxLifetime{ 1.f };

    PROPERTY()
    bool isLooping{ false };

    PROPERTY()
    float spawnRate{ 1.f };

    PROPERTY()
    float duration{ 10.f };
  };

}  // namespace ls::particle
