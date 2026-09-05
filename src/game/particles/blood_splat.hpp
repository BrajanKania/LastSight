#pragma once

#include "engine/gfx/texture_handle.hpp"
#include "engine/particles/particle_emitter_config.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/material/material_handle.hpp"

namespace ls::particle::preset {

  inline particle::ParticleEmitterConfig bloodSplat(
      renderer::MaterialHandle materialHandle, gfx::TextureHandle textureHandle
  ) {
    return particle::ParticleEmitterConfig{
      .materialHandle = materialHandle,
      .textureHandle = textureHandle,

      .layer = renderer::Layer::Entities,
      .maxParticles = 30,

      .minSpeed = 0.5f,
      .maxSpeed = 1.8f,

      .minScale = glm::vec2(0.03f, 0.03f),
      .maxScale = glm::vec2(0.08f, 0.08f),
      .endScale = glm::vec2(0.01f, 0.01f),

      .minRotation = -180.0f,
      .maxRotation = 180.0f,

      .minAngularSpeed = -90.0f,
      .maxAngularSpeed = 90.0f,

      .startColor = glm::vec4(0.7f, 0.02f, 0.05f, 1.0f),
      .endColor = glm::vec4(0.3f, 0.0f, 0.02f, 0.0f),

      .minBrightness = 0.8f,
      .maxBrightness = 1.1f,

      .minLifetime = 0.2f,
      .maxLifetime = 0.4f,

      .isLooping = false,
      .spawnRate = 500.0f,
      .duration = 0.03f,
    };
  }

}  // namespace ls::particle::preset
