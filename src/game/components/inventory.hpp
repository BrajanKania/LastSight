#pragma once

#include <vector>

#include "game/items/item_stack.hpp"
#include "game/items/types.hpp"

namespace ls::component {

  struct Inventory {
    std::size_t maxSlots{ 3 };
    std::vector<item::ItemStack> slots{ maxSlots };
    std::size_t activeSlotIndex{ 0 };

    item::ItemId equippedItemId{};
  };

}  // namespace ls::component
