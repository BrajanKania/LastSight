#include "systems/inventory_system.hpp"

#include <cstdint>

#include "components/inventory.hpp"
#include "components/item_pickup.hpp"
#include "items/item_registry.hpp"
#include "ls/events/interact_with_entity.hpp"

namespace ls::inventory_system {

  void update(const UpdateContext& ctx, const item::ItemRegistry& itemRegistry) {
    for (auto event : ctx.eventQueue.getEvents<event::InteractWithEntity>()) {
      if (!ctx.registry.hasComponent<component::Inventory>(event.interactor) ||
          !ctx.registry.hasComponent<component::ItemPickup>(event.target)) {
        continue;
      }

      auto& itemPickup{ ctx.registry.getComponent<component::ItemPickup>(event.target) };
      auto& inventory{ ctx.registry.getComponent<component::Inventory>(event.interactor) };
      const auto* itemDef{ itemRegistry.get(itemPickup.itemStack.itemId) };

      if (!itemDef) {
        continue;
      }

      if (itemDef->canStack) {
        for (auto& stack : inventory.slots) {
          if (stack.itemId == itemDef->id && stack.count < itemDef->maxStackSize) {
            std::uint32_t spaceLeft{ itemDef->maxStackSize - stack.count };
            std::uint32_t amountToAdd{ std::min(spaceLeft, itemPickup.itemStack.count) };

            stack.count += amountToAdd;
            itemPickup.itemStack.count -= amountToAdd;

            if (itemPickup.itemStack.count == 0) {
              break;
            }
          }
        }
      }

      if (itemPickup.itemStack.count > 0) {
        for (auto& stack : inventory.slots) {
          if (stack.itemId.empty() || stack.count == 0) {
            stack = itemPickup.itemStack;
            itemPickup.itemStack.count = 0;
            break;
          }
        }
      }

      if (itemPickup.itemStack.count == 0) {
        ctx.registry.destroyEntity(event.target);
      }
    }
  }

}  // namespace ls::inventory_system
