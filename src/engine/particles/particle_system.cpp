#include "engine/particles/particle_system.hpp"

#include <cassert>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_common.hpp>
#include <glm/gtc/constants.hpp>
#include <random>
#include <vector>

#include "engine/components/particle_emitter.hpp"
#include "engine/components/transform.hpp"
#include "engine/ecs/types.hpp"
#include "engine/particles/particle.hpp"

namespace ls::particle_system {

  namespace {

    float getRandomFromRange(float min, float max) {
      static std::mt19937 eng{ std::random_device{}() };

      if (min >= max)
        return min;

      std::uniform_real_distribution<float> dist{ min, max };
      return dist(eng);
    }

    void updateParticles(component::ParticleEmitter& emitter, float dt) {
      for (std::size_t i{ 0 }; i < emitter.activeParticlesCount;) {
        emitter.particles[i].lifetime += dt;

        if (emitter.particles[i].lifetime >= emitter.particles[i].maxLifetime) {
          std::swap(emitter.particles[i], emitter.particles[emitter.activeParticlesCount - 1]);
          emitter.activeParticlesCount--;
          continue;
        }

        float t{ emitter.particles[i].lifetime / emitter.particles[i].maxLifetime };
        t = glm::clamp(t, 0.f, 1.f);

        emitter.particles[i].scale = glm::mix(emitter.particles[i].startScale, emitter.particles[i].endScale, t);
        emitter.particles[i].color = glm::mix(emitter.particles[i].startColor, emitter.particles[i].endColor, t);

        emitter.particles[i].position += emitter.particles[i].velocity * dt;
        emitter.particles[i].rotation += emitter.particles[i].angularSpeed * dt;

        i++;
      }
    }

    void spawnParticle(component::ParticleEmitter& emitter, const component::Transform& transform) {
      if (emitter.activeParticlesCount >= emitter.particles.size()) {
        return;
      }

      particle::Particle& p{ emitter.particles[emitter.activeParticlesCount++] };
      p.lifetime = 0.f;
      p.position = transform.position;
      p.maxLifetime = getRandomFromRange(emitter.config.minLifetime, emitter.config.maxLifetime);

      float angle{ getRandomFromRange(0.f, glm::two_pi<float>()) };
      float speed{ getRandomFromRange(emitter.config.minSpeed, emitter.config.maxSpeed) };
      p.velocity = glm::vec2(std::cos(angle), std::sin(angle)) * speed;

      p.startScale = glm::vec2(
          getRandomFromRange(emitter.config.minScale.x, emitter.config.maxScale.x),
          getRandomFromRange(emitter.config.minScale.y, emitter.config.maxScale.y)
      );
      p.endScale = emitter.config.endScale;
      p.scale = p.startScale;

      p.rotation = getRandomFromRange(emitter.config.minRotation, emitter.config.maxRotation);
      p.angularSpeed = getRandomFromRange(emitter.config.minAngularSpeed, emitter.config.maxAngularSpeed);

      float brightness{ getRandomFromRange(emitter.config.minBrightness, emitter.config.maxBrightness) };
      p.startColor = glm::vec4(glm::vec3(emitter.config.startColor) * brightness, emitter.config.startColor.a);
      p.endColor = glm::vec4(glm::vec3(emitter.config.endColor) * brightness, emitter.config.endColor.a);
      p.color = p.startColor;
    }

    void emitParticles(component::ParticleEmitter& emitter, const component::Transform& transform, float dt) {
      if (!emitter.isEmitting || emitter.config.spawnRate <= 0.f) {
        return;
      }

      emitter.spawnTimer += dt;
      float spawnInterval{ 1.f / emitter.config.spawnRate };

      while (emitter.spawnTimer >= spawnInterval) {
        spawnParticle(emitter, transform);
        emitter.spawnTimer -= spawnInterval;
      }

      if (!emitter.config.isLooping && emitter.elapsedTime >= emitter.config.duration) {
        emitter.isEmitting = false;
      }
    }

  }  // namespace

  void update(const UpdateContext& ctx) {
    std::vector<ecs::EntityId> entitiesToDestroy;

    for (ecs::EntityId entity : ctx.registry.view<component::ParticleEmitter, component::Transform>()) {
      auto& emitter{ ctx.registry.getComponent<component::ParticleEmitter>(entity) };
      const auto& transform{ ctx.registry.getComponent<component::Transform>(entity) };

      emitter.elapsedTime += ctx.dt;

      updateParticles(emitter, ctx.dt);
      if (!emitter.isPaused) {
        emitParticles(emitter, transform, ctx.dt);
      }

      if (!emitter.isEmitting && emitter.activeParticlesCount == 0) {
        entitiesToDestroy.push_back(entity);
      }
    }

    for (ecs::EntityId entity : entitiesToDestroy) {
      ctx.registry.destroyEntity(entity);
    }
  }

}  // namespace ls::particle_system
