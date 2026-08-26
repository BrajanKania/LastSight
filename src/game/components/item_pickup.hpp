#pragma once

#include "engine/reflection/reflection_macros.hpp"
#include "game/items/item_stack.hpp"

namespace ls::component {

  COMPONENT()
  struct ItemPickup {
    PROPERTY()
    item::ItemStack itemStack;
  };

}  // namespace ls::component
