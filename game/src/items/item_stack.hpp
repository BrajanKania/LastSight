#pragma once

#include <cstdint>

#include "items/types.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::item {

  STRUCT()
  struct ItemStack {
    PROPERTY()
    ItemId itemId;

    PROPERTY()
    std::uint32_t count{ 0 };
  };

}  // namespace ls::item
