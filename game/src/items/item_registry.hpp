#pragma once

#include <unordered_map>
#include <utility>

#include "items/item_definition.hpp"
#include "items/types.hpp"

namespace ls::item {

  class ItemRegistry {
  public:
    void registerItem(ItemDefinition def) { items_[def.id] = std::move(def); }
    const ItemDefinition* get(const ItemId& id) const {
      auto it{ items_.find(id) };
      return it != items_.end() ? &it->second : nullptr;
    }

  private:
    std::unordered_map<ItemId, ItemDefinition> items_;
  };

}  // namespace ls::item
