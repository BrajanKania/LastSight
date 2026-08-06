#pragma once

#include "game/items/types.hpp"

namespace ls::item {

  struct ItemStack {
    ItemId itemId;
    std::size_t count{ 0 };
  };

}  // namespace ls::item
