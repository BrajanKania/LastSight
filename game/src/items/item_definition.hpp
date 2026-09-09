#pragma once

#include <cstdint>
#include <optional>

#include "items/types.hpp"
#include "items/weapon_config.hpp"
#include "ls/gfx/texture_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::item {

  STRUCT()
  struct ItemDefinition {
    PROPERTY()
    ItemId id;

    PROPERTY()
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY()
    gfx::TextureHandle iconTextureHandle{ 0 };

    PROPERTY()
    gfx::TextureHandle worldTextureHandle{ 0 };

    PROPERTY()
    gfx::TextureHandle equippedTextureHandle{ 0 };

    PROPERTY()
    glm::vec2 equippedScale{ 1.f };

    PROPERTY()
    glm::vec2 equippedOffset{ 0.f };

    PROPERTY()
    float equippedAngleOffset{ 0.f };

    PROPERTY()
    bool canStack{ false };

    PROPERTY()
    std::uint32_t maxStackSize{ 1 };

    PROPERTY()
    std::optional<WeaponConfig> weaponConfig;
  };

}  // namespace ls::item
