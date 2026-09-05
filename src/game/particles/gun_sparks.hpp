#pragma once

#include "engine/gfx/texture_handle.hpp"
#include "engine/particles/particle_emitter_config.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/material/material_handle.hpp"

namespace ls::particle::preset {

  inline particle::ParticleEmitterConfig gunSparks(
      renderer::MaterialHandle materialHandle, gfx::TextureHandle textureHandle
  ) {
    return particle::ParticleEmitterConfig{
      .materialHandle = materialHandle,
      .textureHandle = textureHandle,

      .layer = renderer::Layer::Entities,
      .maxParticles = 30,

      .minSpeed = 0.5f,
      .maxSpeed = 1.f,

      .minScale = glm::vec2(0.02f, 0.02f),
      .maxScale = glm::vec2(0.06f, 0.06f),
      .endScale = glm::vec2(0.0f, 0.0f),

      .minRotation = -180.0f,
      .maxRotation = 180.0f,

      .minAngularSpeed = -180.0f,
      .maxAngularSpeed = 180.0f,

      .startColor = glm::vec4(1.0f, 0.65f, 0.2f, 1.0f),
      .endColor = glm::vec4(1.0f, 0.1f, 0.0f, 0.0f),

      .minBrightness = 1.0f,
      .maxBrightness = 1.6f,

      .minLifetime = 0.1f,
      .maxLifetime = 0.2f,

      .isLooping = false,
      .spawnRate = 500.0f,
      .duration = 0.03f,
    };
  }

}  // namespace ls::particle::preset
