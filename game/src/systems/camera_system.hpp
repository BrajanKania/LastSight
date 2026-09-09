#pragma once

#include <glm/ext/vector_float2.hpp>

#include "ls/components/camera.hpp"
#include "ls/core/update_context.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"

namespace ls::camera_system {

  void update(const UpdateContext& ctx);

  void follow(const UpdateContext& ctx, const ecs::EntityId targetEntity, float defaultSmoothness = 1.f);

  glm::vec2 screenToWorld(glm::vec2 mousePosition, glm::vec2 viewportSize, const component::Camera& camera);

  void addImpulse(ecs::Registry& registry, const ecs::EntityId cameraEntity, glm::vec2 impulse);

  void addTrauma(ecs::Registry& registry, const ecs::EntityId cameraEntity, float amount);

}  // namespace ls::camera_system
