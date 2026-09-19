#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

#include "ls/asset/asset_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/layer.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::particle {

  STRUCT(DisplayName("Particle Emitter Config"))
  struct ParticleEmitterConfig {
    PROPERTY(DisplayName("Texture"))
    asset::TextureHandle textureHandle{ asset::kNullAsset };

    PROPERTY(DisplayName("Material"))
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY(DisplayName("Layer"))
    renderer::Layer layer{ renderer::Layer::Entities };

    PROPERTY(DisplayName("Max Particles"), ReadOnly)
    std::size_t maxParticles{ 10 };

    PROPERTY(DisplayName("Min Speed"))
    float minSpeed{ 0.f };

    PROPERTY(DisplayName("Max Speed"))
    float maxSpeed{ 1.f };

    PROPERTY(DisplayName("Min Scale"))
    glm::vec2 minScale{ 0.5f };

    PROPERTY(DisplayName("Max Scale"))
    glm::vec2 maxScale{ 1.f };

    PROPERTY(DisplayName("End Scale"))
    glm::vec2 endScale{ 0.f };

    PROPERTY(DisplayName("Min Rotation"))
    float minRotation{ -180.f };

    PROPERTY(DisplayName("Max Rotation"))
    float maxRotation{ 180.f };

    PROPERTY(DisplayName("Min Angular Speed"))
    float minAngularSpeed{ 0.f };

    PROPERTY(DisplayName("Max Angular Speed"))
    float maxAngularSpeed{ 90.f };

    PROPERTY(DisplayName("Start Color"), color)
    glm::vec4 startColor{ 1.f };

    PROPERTY(DisplayName("End Color"), color)
    glm::vec4 endColor{ 0.f };

    PROPERTY(DisplayName("Min Brightness"))
    float minBrightness{ 0.7f };

    PROPERTY(DisplayName("Max Brightness"))
    float maxBrightness{ 1.3f };

    PROPERTY(DisplayName("Min Lifetime"))
    float minLifetime{ 0.1f };

    PROPERTY(DisplayName("Max Lifetime"))
    float maxLifetime{ 1.f };

    PROPERTY(DisplayName("Is Looping"))
    bool isLooping{ false };

    PROPERTY(DisplayName("Spawn Rate"))
    float spawnRate{ 1.f };

    PROPERTY(DisplayName("Duration"))
    float duration{ 10.f };
  };

}  // namespace ls::particle
