#pragma once

#include <cstdint>
#include <vector>

#include "ls/particles/particle.hpp"
#include "ls/particles/particle_emitter_config.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Particle Emitter"))
  struct ParticleEmitter {
    PROPERTY(DisplayName("Config"))
    particle::ParticleEmitterConfig config;

    PROPERTY(DisplayName("Elapsed Time"), ReadOnly, Transient)
    float elapsedTime{ 0.f };

    PROPERTY(DisplayName("Is Paused"))
    bool isPaused{ false };

    PROPERTY(DisplayName("Is Emitting"))
    bool isEmitting{ true };

    PROPERTY(DisplayName("Spawn Timer"), ReadOnly, Transient)
    float spawnTimer{ 0.f };

    PROPERTY(DisplayName("Particles"), Transient)
    std::vector<particle::Particle> particles;

    PROPERTY(DisplayName("Active Particle Count"), ReadOnly, Transient)
    std::uint32_t activeParticlesCount{ 0 };

    ParticleEmitter() = default;

    explicit ParticleEmitter(const particle::ParticleEmitterConfig& config)
        : config{ config } {
      particles.resize(config.maxParticles);
    }
  };

}  // namespace ls::component
