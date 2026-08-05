#pragma once

#include <vector>

#include "engine/particles/particle.hpp"
#include "engine/particles/particle_emitter_config.hpp"

namespace ls::component {

  struct ParticleEmitter {
    particle::ParticleEmitterConfig config;

    float elapsedTime{ 0.f };

    bool isEmitting{ true };
    float spawnTimer{ 0.f };

    std::vector<particle::Particle> particles;
    std::size_t activeParticlesCount{ 0 };

    ParticleEmitter() = default;

    explicit ParticleEmitter(const particle::ParticleEmitterConfig& config)
        : config{ config } {
      particles.resize(config.maxParticles);
    }
  };

}  // namespace ls::component
