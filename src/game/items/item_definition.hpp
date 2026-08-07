#pragma once

#include <cstdint>
#include <optional>

#include "game/items/types.hpp"
#include "game/items/weapon_config.hpp"

namespace ls::item {

  struct ItemDefinition {
    ItemId id;

    uint32_t iconTextureId{ 0 };
    uint32_t worldTextureId{ 0 };
    uint32_t equippedTextureId{ 0 };

    glm::vec2 equippedScale{ 1.f };
    glm::vec2 equippedOffset{ 0.f };
    float equippedAngleOffset{ 0.f };

    bool canStack{ false };
    std::size_t maxStackSize{ 1 };

    std::optional<WeaponConfig> weaponConfig;
  };

}  // namespace ls::item
