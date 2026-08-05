#pragma once

#include "engine/particles/particle_emitter_config.hpp"

namespace ls::particle::preset {

  inline particle::ParticleEmitterConfig fire() {
    return particle::ParticleEmitterConfig{
      .maxParticles = 80,

      .minSpeed = 0.2f,
      .maxSpeed = 1.8f,

      .minScale = glm::vec2(0.08f, 0.08f),
      .maxScale = glm::vec2(0.15f, 0.15f),
      .endScale = glm::vec2(0.01f, 0.01f),

      .minRotation = -180.0f,
      .maxRotation = 180.0f,

      .minAngularSpeed = -60.0f,
      .maxAngularSpeed = 60.0f,

      .startColor = glm::vec4(1.0f, 0.75f, 0.1f, 0.9f),
      .endColor = glm::vec4(0.8f, 0.15f, 0.02f, 0.0f),

      .minBrightness = 0.9f,
      .maxBrightness = 1.5f,

      .minLifetime = 0.3f,
      .maxLifetime = 0.6f,

      .isLooping = true,
      .spawnRate = 35.0f,
      .duration = 0.0f,
    };
  }

}  // namespace ls::particle::preset
