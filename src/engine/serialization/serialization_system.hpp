#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "engine/gfx/texture_manager.hpp"
#include "engine/prefab/prefab_manager.hpp"
#include "engine/renderer/material/material_manager.hpp"

namespace ls::serialization_system {

  nlohmann::json serializeReflected(
      entt::meta_any any,
      const renderer::MaterialManager& materialManager,
      const gfx::TextureManager& textureManager,
      const prefab::PrefabManager& prefabManager
  );

  entt::meta_any deserializeReflected(
      entt::meta_type type,
      const nlohmann::json& jsonValue,
      const renderer::MaterialManager& materialManager,
      const gfx::TextureManager& textureManager,
      const prefab::PrefabManager& prefabManager
  );

}  // namespace ls::serialization_system
