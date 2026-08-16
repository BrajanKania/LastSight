#pragma once

#include <cstdint>
#include <vector>

#include "engine/particles/particle.hpp"
#include "engine/particles/particle_emitter_config.hpp"
#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct ParticleEmitter {
    particle::ParticleEmitterConfig config;

    PROPERTY(ReadOnly, Transient)
    float elapsedTime{ 0.f };

    bool isPaused{ false };
    bool isEmitting{ true };

    PROPERTY(ReadOnly, Transient)
    float spawnTimer{ 0.f };

    PROPERTY(Transient)
    std::vector<particle::Particle> particles;

    PROPERTY(ReadOnly)
    std::uint32_t activeParticlesCount{ 0 };

    ParticleEmitter() = default;

    explicit ParticleEmitter(const particle::ParticleEmitterConfig& config)
        : config{ config } {
      particles.resize(config.maxParticles);
    }
  };

}  // namespace ls::component
