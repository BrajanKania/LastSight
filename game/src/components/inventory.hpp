#pragma once

#include <cstdint>
#include <vector>

#include "items/item_stack.hpp"
#include "items/types.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Inventory {
    PROPERTY(ReadOnly)
    uint32_t maxSlots{ 3 };

    PROPERTY()
    std::vector<item::ItemStack> slots;

    PROPERTY(ReadOnly)
    std::size_t activeSlotIndex{ 0 };

    PROPERTY(ReadOnly)
    item::ItemId equippedItemId{};

    explicit Inventory(uint32_t maxSlotsCount = 3)
        : maxSlots(maxSlotsCount),
          slots(maxSlotsCount) {}
  };

}  // namespace ls::component
