#pragma once

#include <cstdint>
#include <optional>

#include "engine/gfx/texture_handle.hpp"
#include "engine/reflection/reflection_macros.hpp"
#include "game/items/types.hpp"
#include "game/items/weapon_config.hpp"

namespace ls::item {

  STRUCT()
  struct ItemDefinition {
    ItemId id;

    gfx::TextureHandle iconTextureHandle{ 0 };
    gfx::TextureHandle worldTextureHandle{ 0 };
    gfx::TextureHandle equippedTextureHandle{ 0 };

    glm::vec2 equippedScale{ 1.f };
    glm::vec2 equippedOffset{ 0.f };
    float equippedAngleOffset{ 0.f };

    bool canStack{ false };
    std::uint32_t maxStackSize{ 1 };

    std::optional<WeaponConfig> weaponConfig;
  };

}  // namespace ls::item
