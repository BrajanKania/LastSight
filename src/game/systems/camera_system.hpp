#pragma once

#include "engine/core/update_context.hpp"
#include "engine/ecs/types.hpp"
#include "game/components/camera.hpp"

namespace ls::camera_system {

  void update(const UpdateContext& ctx);

  void follow(const UpdateContext& ctx, const ecs::EntityId targetEntity, float defaultSmoothness = 1.f);

  glm::vec2 screenToWorld(glm::vec2 mousePosition, glm::vec2 viewportSize, const component::Camera& camera);

}  // namespace ls::camera_system
