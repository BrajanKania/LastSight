#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/ecs/registry.hpp"

namespace ls::factory {

  struct ProjectileConfig {
    glm::vec2 scale{ 1.f };
    glm::vec2 position{ 0.f };
    glm::vec2 direction{ 1.f, 0.f };
    float speed{ 0.f };

    uint32_t textureId{ 0 };
    float angleOffset{ 0.f };

    float lifetime{ 0.f };
  };

  void spawnProjectile(ecs::Registry& registry, const ProjectileConfig& config);

}  // namespace ls::factory
