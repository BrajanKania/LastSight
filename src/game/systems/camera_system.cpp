#include "game/systems/camera_system.hpp"

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/components/transform.hpp"
#include "engine/renderer/renderer_system.hpp"
#include "game/components/camera.hpp"

namespace ls::camera_system {

  void update(ecs::Registry& registry) {
    glm::vec2 viewportSize{renderer_system::getViewportSize()};
    float aspectRatio{(viewportSize.y > 0.f ? viewportSize.x / viewportSize.y : 1.f)};

    for (auto entity : registry.view<component::Camera, component::Transform>()) {
      auto& camera{registry.getComponent<component::Camera>(entity)};
      const auto& transform{registry.getComponent<component::Transform>(entity)};

      float halfHeight{(camera.orthographicSize / camera.zoom) * 0.5f};
      float halfWidth{halfHeight * aspectRatio};

      camera.projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

      glm::mat4 cameraModel{1.0f};
      cameraModel = glm::translate(cameraModel, glm::vec3(transform.position, 0.0f));
      cameraModel = glm::rotate(cameraModel, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
      camera.view = glm::inverse(cameraModel);
    }
  }

  void follow(ecs::Registry& registry, const ecs::Entity targetEntity, float dt, float smoothness) {
    if (!registry.hasComponent<component::Transform>(targetEntity))
      return;

    const auto& targetTransform{registry.getComponent<component::Transform>(targetEntity)};

    for (auto entity : registry.view<component::Camera, component::Transform>()) {
      auto& cameraTransform{registry.getComponent<component::Transform>(entity)};
      cameraTransform.position = glm::mix(cameraTransform.position, targetTransform.position, smoothness * dt);
    }
  }

}  // namespace ls::camera_system
