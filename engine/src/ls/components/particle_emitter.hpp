#pragma once

#include <cstdint>
#include <vector>

#include "ls/particles/particle.hpp"
#include "ls/particles/particle_emitter_config.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct ParticleEmitter {
    PROPERTY()
    particle::ParticleEmitterConfig config;

    PROPERTY(ReadOnly, Transient)
    float elapsedTime{ 0.f };

    PROPERTY()
    bool isPaused{ false };

    PROPERTY()
    bool isEmitting{ true };

    PROPERTY(ReadOnly, Transient)
    float spawnTimer{ 0.f };

    PROPERTY(Transient)
    std::vector<particle::Particle> particles;

    PROPERTY(ReadOnly, Transient)
    std::uint32_t activeParticlesCount{ 0 };

    ParticleEmitter() = default;

    explicit ParticleEmitter(const particle::ParticleEmitterConfig& config)
        : config{ config } {
      particles.resize(config.maxParticles);
    }
  };

}  // namespace ls::component
