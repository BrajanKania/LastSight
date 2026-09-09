#include "systems/player_system.hpp"

#include <SDL3/SDL_log.h>

#include <algorithm>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include "actions/aim.hpp"
#include "actions/interact.hpp"
#include "actions/move.hpp"
#include "actions/select_slot_0.hpp"
#include "actions/select_slot_1.hpp"
#include "actions/select_slot_2.hpp"
#include "actions/select_slot_3.hpp"
#include "actions/select_slot_4.hpp"
#include "actions/select_slot_5.hpp"
#include "actions/shoot.hpp"
#include "actions/sprint.hpp"
#include "components/inventory.hpp"
#include "components/movement_settings.hpp"
#include "components/player.hpp"
#include "components/player_state.hpp"
#include "components/stamina.hpp"
#include "components/weapon.hpp"
#include "events/request_shoot.hpp"
#include "ls/components/camera.hpp"
#include "ls/components/transform.hpp"
#include "ls/components/velocity.hpp"
#include "ls/core/update_context.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/events/request_interaction.hpp"
#include "ls/input/input_system.hpp"
#include "ls/input/types.hpp"
#include "systems/camera_system.hpp"

namespace ls::player_system {

  namespace {

    void updateMovement(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      auto& playerState{ ctx.registry.getComponent<component::PlayerState>(playerEntity) };
      auto& velocity{ ctx.registry.getComponent<component::Velocity>(playerEntity) };
      const auto& movement{ ctx.registry.getComponent<component::MovementSettings>(playerEntity) };
      const auto& stamina{ ctx.registry.getComponent<component::Stamina>(playerEntity) };

      auto sprintActionState{ ctx.inputManager.getActionState<action::Sprint>() };
      bool wantSprint{ sprintActionState == input::ActionState::JustPressed ||
                       sprintActionState == input::ActionState::Held };

      bool canSprint{ !stamina.isExhausted && stamina.current > 0.f };

      glm::vec2 direction{ ctx.inputManager.getAxis2D<action::Move>() };
      float dirLength{ glm::length(direction) };

      float speed{ 0.f };
      if (dirLength > 0.f && wantSprint && canSprint) {
        speed = movement.sprintSpeed;
        playerState.isSprinting = true;
      } else {
        speed = movement.walkSpeed;
        playerState.isSprinting = false;
      }

      playerState.isMoving = dirLength > 0.f;
      velocity.linear = direction * speed;
    }

    void handleRotation(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      const auto& movement{ ctx.registry.getComponent<component::MovementSettings>(playerEntity) };
      const auto& transform{ ctx.registry.getComponent<component::Transform>(playerEntity) };
      auto& velocity{ ctx.registry.getComponent<component::Velocity>(playerEntity) };

      for (auto entity : ctx.registry.view<component::Camera>()) {
        const auto& camera{ ctx.registry.getComponent<component::Camera>(entity) };

        glm::vec2 mouseWorldPos{ camera_system::screenToWorld(
            input_system::getViewportMousePosition(), input_system::getViewportSize(), camera
        ) };

        glm::vec2 direction{ mouseWorldPos - transform.position };
        if (glm::length(direction) > 0.001f) {
          float targetAngle{ glm::degrees(std::atan2(direction.y, direction.x)) };
          float angleDiff{ std::remainder(targetAngle - transform.rotation, 360.f) };
          float desiredAngularSpeed{ (ctx.dt > 0.00001f ? angleDiff / ctx.dt : 0.f) };
          float maxAngularSpeed{ movement.angularSpeed };
          velocity.angular = glm::clamp(desiredAngularSpeed, -maxAngularSpeed, maxAngularSpeed);
        } else {
          velocity.angular = 0.f;
        }
        break;
      }
    }

    void updateAim(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      auto& playerState{ ctx.registry.getComponent<component::PlayerState>(playerEntity) };
      const auto& stamina{ ctx.registry.getComponent<component::Stamina>(playerEntity) };
      const auto& aimActionState{ ctx.inputManager.getActionState<action::Aim>() };

      bool hasWeapon{ ctx.registry.hasComponent<component::Weapon>(playerEntity) };
      bool wantsToAim{ aimActionState == input::ActionState::JustPressed ||
                       aimActionState == input::ActionState::Held };
      bool canAim{ !stamina.isExhausted && stamina.current > 0.f };

      playerState.isAiming = hasWeapon && wantsToAim && canAim;

      if (!hasWeapon) {
        return;
      }

      auto& weapon{ ctx.registry.getComponent<component::Weapon>(playerEntity) };

      weapon.isAiming = playerState.isAiming;
      auto shootActionState{ ctx.inputManager.getActionState<action::Shoot>() };
      bool wantsToShoot{ weapon.isAutomatic ? (shootActionState == input::ActionState::JustPressed ||
                                               shootActionState == input::ActionState::Held)
                                            : (shootActionState == input::ActionState::JustPressed) };

      if (wantsToShoot) {
        ctx.eventQueue.publish(
            event::RequestShoot{
                .shooter = playerEntity,
            }
        );
      }
    }

    void updateInteractions(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      if (ctx.inputManager.getActionState<action::Interact>() == input::ActionState::JustPressed) {
        ctx.eventQueue.publish(
            event::RequestInteraction{
                .interactor = playerEntity,
            }
        );
      }
    }

    void updateSlotSelection(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      auto& inventory{ ctx.registry.getComponent<component::Inventory>(playerEntity) };
      int slotCount{ static_cast<int>(inventory.slots.size()) };

      if (ctx.inputManager.getActionState<action::SelectSlot0>() == input::ActionState::JustPressed && slotCount >= 1) {
        inventory.activeSlotIndex = 0;
      }
      if (ctx.inputManager.getActionState<action::SelectSlot1>() == input::ActionState::JustPressed && slotCount >= 2) {
        inventory.activeSlotIndex = 1;
      }
      if (ctx.inputManager.getActionState<action::SelectSlot2>() == input::ActionState::JustPressed && slotCount >= 3) {
        inventory.activeSlotIndex = 2;
      }
      if (ctx.inputManager.getActionState<action::SelectSlot3>() == input::ActionState::JustPressed && slotCount >= 4) {
        inventory.activeSlotIndex = 3;
      }
      if (ctx.inputManager.getActionState<action::SelectSlot4>() == input::ActionState::JustPressed && slotCount >= 5) {
        inventory.activeSlotIndex = 4;
      }
      if (ctx.inputManager.getActionState<action::SelectSlot5>() == input::ActionState::JustPressed && slotCount >= 6) {
        inventory.activeSlotIndex = 5;
      }

      float scrollDelta{ ctx.inputManager.getScrollDelta() };
      if (scrollDelta == 0.f) {
        return;
      }

      if (inventory.slots.empty()) {
        return;
      }

      int step{ scrollDelta < 0.f ? 1 : -1 };
      int activeIndex{ static_cast<int>(inventory.activeSlotIndex) };

      int newIndex{ (activeIndex + step) % slotCount };
      if (newIndex < 0) {
        newIndex += slotCount;
      }

      inventory.activeSlotIndex = static_cast<std::size_t>(newIndex);
    }

    void updateStamina(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      auto& stamina{ ctx.registry.getComponent<component::Stamina>(playerEntity) };
      const auto& playerState{ ctx.registry.getComponent<component::PlayerState>(playerEntity) };

      if (playerState.isSprinting) {
        stamina.current -= stamina.sprintCostRate * ctx.dt;
      }
      if (playerState.isAiming) {
        stamina.current -= stamina.aimCostRate * ctx.dt;
      }

      if (!playerState.isSprinting && !playerState.isAiming) {
        stamina.current += stamina.regenRate * ctx.dt;
      }

      stamina.current = std::clamp(stamina.current, 0.f, stamina.max);

      if (stamina.current <= 0.f) {
        stamina.isExhausted = true;
      } else if (stamina.isExhausted && stamina.current >= (stamina.max * stamina.recoveryThresholdRatio)) {
        stamina.isExhausted = false;
      }
    }

  }  // namespace

  void update(const UpdateContext& ctx) {
    for (auto entity : ctx.registry.view<
                       component::Player,
                       component::PlayerState,
                       component::Velocity,
                       component::MovementSettings,
                       component::Stamina>()) {
      updateMovement(ctx, entity);
      handleRotation(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::PlayerState, component::Transform>()) {
      updateInteractions(ctx, entity);
    }

    for (auto entity :
         ctx.registry.view<component::Player, component::PlayerState, component::Transform, component::Stamina>()) {
      updateAim(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::Inventory>()) {
      updateSlotSelection(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::PlayerState, component::Stamina>()) {
      updateStamina(ctx, entity);
    }
  }

}  // namespace ls::player_system
