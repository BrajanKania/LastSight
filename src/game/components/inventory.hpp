#pragma once

#include <vector>

#include "engine/reflection/reflection_macros.hpp"
#include "game/items/item_stack.hpp"
#include "game/items/types.hpp"

namespace ls::component {

  PROPERTY()
  struct Inventory {
    PROPERTY(ReadOnly)
    std::size_t maxSlots{ 3 };

    std::vector<item::ItemStack> slots{ maxSlots };

    PROPERTY(ReadOnly)
    std::size_t activeSlotIndex{ 0 };

    PROPERTY(ReadOnly)
    item::ItemId equippedItemId{};
  };

}  // namespace ls::component
