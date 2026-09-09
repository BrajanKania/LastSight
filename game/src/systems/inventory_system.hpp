#pragma once

#include "items/item_registry.hpp"
#include "ls/core/update_context.hpp"

namespace ls::inventory_system {

  void update(const UpdateContext& ctx, const item::ItemRegistry& itemRegistry);

}
