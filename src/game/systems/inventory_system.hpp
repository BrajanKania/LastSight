#pragma once

#include "engine/core/update_context.hpp"
#include "game/items/item_registry.hpp"

namespace ls::inventory_system {

  void update(const UpdateContext& ctx, const item::ItemRegistry& itemRegistry);

}
