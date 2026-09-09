#pragma once

#include "items/item_stack.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct ItemPickup {
    PROPERTY()
    item::ItemStack itemStack;
  };

}  // namespace ls::component
