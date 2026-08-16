#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "engine/gfx/texture_manager.hpp"

namespace ls::serialization_system {

  nlohmann::json serializeReflected(entt::meta_any any, const gfx::TextureManager& textureManager);

  entt::meta_any deserializeReflected(
      entt::meta_type type, const nlohmann::json& jsonValue, const gfx::TextureManager& textureManager
  );

}  // namespace ls::serialization_system
