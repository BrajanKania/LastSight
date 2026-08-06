#include "game/systems/player_system.hpp"

#include <SDL3/SDL_log.h>

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/update_context.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/request_interaction.hpp"
#include "engine/input/types.hpp"
#include "engine/renderer/render_system.hpp"
#include "game/actions/interact.hpp"
#include "game/actions/move.hpp"
#include "game/actions/select_slot_0.hpp"
#include "game/actions/select_slot_1.hpp"
#include "game/actions/select_slot_2.hpp"
#include "game/actions/select_slot_3.hpp"
#include "game/actions/select_slot_4.hpp"
#include "game/actions/select_slot_5.hpp"
#include "game/actions/shoot.hpp"
#include "game/actions/sprint.hpp"
#include "game/components/camera.hpp"
#include "game/components/inventory.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"
#include "game/components/weapon.hpp"
#include "game/systems/combat_system.hpp"

namespace ls::player_system {

  namespace {

    void updateMovement(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      auto& velocity{ ctx.registry.getComponent<component::Velocity>(playerEntity) };
      const auto& movement{ ctx.registry.getComponent<component::Movement>(playerEntity) };

      auto sprintActionState{ ctx.inputManager.getActionState<action::Sprint>() };
      float speed{ (sprintActionState == input::ActionState::JustPressed ||
                    sprintActionState == input::ActionState::Held)
                       ? movement.runSpeed
                       : movement.walkSpeed };

      glm::vec2 direction{ ctx.inputManager.getAxis2D<action::Move>() };
      velocity.linear = direction * speed;
    }

    glm::vec2 screenToWorld(glm::vec2 mousePos, glm::vec2 viewportSize, const glm::mat4& invViewProjection) {
      float ndcX{ (2.0f * mousePos.x) / viewportSize.x - 1.0f };
      float ndcY{ 1.0f - (2.0f * mousePos.y) / viewportSize.y };

      glm::vec4 worldPos{ invViewProjection * glm::vec4(ndcX, ndcY, 0.0f, 1.0f) };
      return glm::vec2(worldPos.x, worldPos.y);
    }

    void handleRotation(const UpdateContext& ctx, const ecs::EntityId playerEntity) {
      const auto& movement{ ctx.registry.getComponent<component::Movement>(playerEntity) };
      const auto& transform{ ctx.registry.getComponent<component::Transform>(playerEntity) };
      auto& velocity{ ctx.registry.getComponent<component::Velocity>(playerEntity) };

      for (auto entity : ctx.registry.view<component::Camera>()) {
        const auto& camera{ ctx.registry.getComponent<component::Camera>(entity) };

        glm::mat4 invViewProjection{ glm::inverse(camera.projection * camera.view) };
        glm::vec2 viewportSize{ render_system::getViewportSize() };
        glm::vec2 mouseScreenPos{ ctx.inputManager.getMousePosition() };

        glm::vec2 mouseWorldPos{ screenToWorld(mouseScreenPos, viewportSize, invViewProjection) };

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
      const auto& weapon{ ctx.registry.getComponent<component::Weapon>(playerEntity) };

      if (weapon.cooldown <= 0.f) {
        auto actionState{ ctx.inputManager.getActionState<action::Shoot>() };
        if (weapon.isAutomatic
                ? actionState == input::ActionState::JustPressed || actionState == input::ActionState::Held
                : actionState == input::ActionState::JustPressed) {
          combat_system::shoot(ctx, playerEntity);
        }
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

  }  // namespace

  void update(const UpdateContext& ctx) {
    for (auto entity : ctx.registry.view<component::Player, component::Velocity, component::Movement>()) {
      updateMovement(ctx, entity);
      handleRotation(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::Transform, component::Weapon>()) {
      updateAim(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::Transform>()) {
      updateInteractions(ctx, entity);
    }

    for (auto entity : ctx.registry.view<component::Player, component::Inventory>()) {
      updateSlotSelection(ctx, entity);
    }
  }

}  // namespace ls::player_system
