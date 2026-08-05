#include "engine/interactions/interaction_system.hpp"

#include <glm/geometric.hpp>
#include <limits>

#include "engine/components/interactable.hpp"
#include "engine/components/transform.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/interact_with_entity.hpp"
#include "engine/events/request_interaction.hpp"

namespace ls::interaction_system {

  void update(const UpdateContext& ctx) {
    for (auto event : ctx.eventQueue.getEvents<event::RequestInteraction>()) {
      const auto& interactorTransform{ ctx.registry.getComponent<component::Transform>(event.interactor) };

      ecs::EntityId bestTarget{ ecs::kNullEntity };
      float minDistance{ std::numeric_limits<float>::max() };

      for (auto targetEntity : ctx.registry.view<component::Interactable, component::Transform>()) {
        const auto& targetTransform{ ctx.registry.getComponent<component::Transform>(targetEntity) };
        const auto& targetInteractable{ ctx.registry.getComponent<component::Interactable>(targetEntity) };

        float dist{ glm::length(interactorTransform.position - targetTransform.position) };
        if (dist < targetInteractable.radius && dist < minDistance) {
          bestTarget = targetEntity;
          minDistance = dist;
        }
      }

      if (bestTarget != ecs::kNullEntity) {
        ctx.eventQueue.publish(
            event::InteractWithEntity{
                .interactor = event.interactor,
                .target = bestTarget,
            }
        );
      }
    }
  }

}  // namespace ls::interaction_system
