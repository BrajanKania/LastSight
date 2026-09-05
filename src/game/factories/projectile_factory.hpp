#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/ecs/registry.hpp"
#include "engine/gfx/texture_handle.hpp"
#include "engine/renderer/material/material_handle.hpp"

namespace ls::factory {

  struct ProjectileConfig {
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };
    gfx::TextureHandle textureHandle{ 0 };

    glm::vec2 scale{ 1.f };
    glm::vec2 position{ 0.f };
    glm::vec2 direction{ 1.f, 0.f };
    float speed{ 0.f };

    float angleOffset{ 0.f };

    float lifetime{ 0.f };
  };

  void spawnProjectile(ecs::Registry& registry, const ProjectileConfig& config);

}  // namespace ls::factory
