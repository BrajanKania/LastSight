#pragma once

#include <glm/ext/vector_float2.hpp>

#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::factory {

  struct ProjectileConfig {
    asset::TextureHandle textureHandle{ asset::kNullAsset };
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    glm::vec2 scale{ 1.f };
    glm::vec2 position{ 0.f };
    glm::vec2 direction{ 1.f, 0.f };
    float speed{ 0.f };

    float angleOffset{ 0.f };

    float lifetime{ 0.f };
  };

  void spawnProjectile(ecs::Registry& registry, const ProjectileConfig& config);

}  // namespace ls::factory
