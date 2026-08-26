#pragma once

#include <cstdint>

#include "engine/reflection/reflection_macros.hpp"
#include "game/items/types.hpp"

namespace ls::item {

  STRUCT()
  struct ItemStack {
    PROPERTY()
    ItemId itemId;

    PROPERTY()
    std::uint32_t count{ 0 };
  };

}  // namespace ls::item
