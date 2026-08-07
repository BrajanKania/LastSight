#include "game/systems/equip_system.hpp"

#include "engine/components/equipped_sprite.hpp"
#include "engine/renderer/layer.hpp"
#include "game/components/inventory.hpp"
#include "game/components/player.hpp"
#include "game/components/weapon.hpp"

namespace ls::equip_system {

  void update(const UpdateContext& ctx, const item::ItemRegistry& itemRegistry) {
    for (auto entity : ctx.registry.view<component::Player, component::Inventory>()) {
      auto& inventory{ ctx.registry.getComponent<component::Inventory>(entity) };

      const auto& currentStack{ inventory.slots[inventory.activeSlotIndex] };

      if (currentStack.itemId == inventory.equippedItemId) {
        continue;
      }

      if (ctx.registry.hasComponent<component::Weapon>(entity)) {
        ctx.registry.destroyComponent<component::Weapon>(entity);
      }
      if (ctx.registry.hasComponent<component::EquippedSprite>(entity)) {
        ctx.registry.destroyComponent<component::EquippedSprite>(entity);
      }

      const auto* itemDef{ itemRegistry.get(currentStack.itemId) };
      if (itemDef != nullptr) {
        if (itemDef->equippedTextureId != 0) {
          ctx.registry.addComponent(
              entity,
              component::EquippedSprite{
                  .textureId = itemDef->equippedTextureId,
                  .scale = itemDef->equippedScale,
                  .offset = itemDef->weaponConfig->hipOffset,
                  .zIndex = renderer::Layer::Entities,
              }
          );
        }

        if (itemDef->weaponConfig.has_value()) {
          ctx.registry.addComponent(
              entity,
              component::Weapon{
                  .isAutomatic = itemDef->weaponConfig->isAutomatic,
                  .fireRate = itemDef->weaponConfig->fireRate,
                  .initialSpeed = itemDef->weaponConfig->initialSpeed,
                  .bulletScale = itemDef->weaponConfig->bulletScale,
                  .bulletLifetime = itemDef->weaponConfig->bulletLifetime,
                  .transitionSpeed = itemDef->weaponConfig->transitionSpeed,
                  .baseAngleOffset = itemDef->equippedAngleOffset,
                  .offset = itemDef->weaponConfig->hipOffset,
                  .muzzleOffset = itemDef->weaponConfig->muzzleOffset,
                  .hipOffset = itemDef->weaponConfig->hipOffset,
                  .aimOffset = itemDef->weaponConfig->aimOffset,
                  .hipRecoil = itemDef->weaponConfig->hipRecoil,
                  .aimRecoil = itemDef->weaponConfig->aimRecoil,
              }
          );
        }
      }

      inventory.equippedItemId = currentStack.itemId;
    }
  }

}  // namespace ls::equip_system
