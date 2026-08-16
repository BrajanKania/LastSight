#include "game/systems/camera_system.hpp"

#include <algorithm>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_common.hpp>
#include <glm/ext/vector_common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

#include "engine/components/transform.hpp"
#include "engine/core/update_context.hpp"
#include "engine/input/input_system.hpp"
#include "game/components/camera.hpp"
#include "game/components/camera_shake.hpp"
#include "game/components/player.hpp"
#include "game/components/player_camera_settings.hpp"
#include "game/components/player_state.hpp"

namespace ls::camera_system {

  void update(const UpdateContext& ctx) {
    glm::vec2 viewportSize{ input_system::getViewportSize() };
    float aspectRatio{ (viewportSize.y > 0.f ? viewportSize.x / viewportSize.y : 1.f) };

    for (auto entity : ctx.registry.view<component::Camera, component::Transform>()) {
      auto& camera{ ctx.registry.getComponent<component::Camera>(entity) };
      const auto& transform{ ctx.registry.getComponent<component::Transform>(entity) };

      glm::vec2 totalShakeOffset{ 0.f };

      if (ctx.registry.hasComponent<component::CameraShake>(entity)) {
        auto& shake{ ctx.registry.getComponent<component::CameraShake>(entity) };
        shake.trauma = glm::clamp(shake.trauma, 0.f, 1.f);

        glm::vec2 springForce{ -shake.stiffness * shake.offset };
        glm::vec2 dampingForce{ -shake.damping * shake.velocity };
        glm::vec2 acceleration{ springForce + dampingForce };

        shake.velocity += acceleration * ctx.dt;
        shake.offset += shake.velocity * ctx.dt;

        if (glm::length(shake.offset) > shake.maxSpringOffset) {
          shake.offset = glm::normalize(shake.offset) * shake.maxSpringOffset;
        }

        totalShakeOffset += shake.offset;

        if (shake.trauma > 0.f) {
          shake.timeAccumulator += ctx.dt * shake.traumaFrequency;
          float shakeAmount{ shake.trauma * shake.trauma };

          float noiseX{ std::sin(shake.timeAccumulator * 1.1f) };
          float noiseY{ std::cos(shake.timeAccumulator * 1.4f) };

          glm::vec2 traumaOffset{ noiseX * shake.maxTraumaOffset * shakeAmount,
                                  noiseY * shake.maxTraumaOffset * shakeAmount };

          totalShakeOffset += traumaOffset;
          shake.trauma = std::max(0.f, shake.trauma - shake.traumaDecay * ctx.dt);
        }
      }

      float halfHeight{ (camera.orthographicSize / camera.zoom) * 0.5f };
      float halfWidth{ halfHeight * aspectRatio };

      camera.projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

      glm::mat4 cameraModel{ 1.0f };
      cameraModel = glm::translate(cameraModel, glm::vec3(transform.position + totalShakeOffset, 0.0f));
      cameraModel = glm::rotate(cameraModel, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
      camera.view = glm::inverse(cameraModel);
    }
  }

  void follow(const UpdateContext& ctx, const ecs::EntityId targetEntity, float smoothness) {
    if (!ctx.registry.hasComponent<component::Transform>(targetEntity))
      return;

    for (auto entity : ctx.registry.view<component::Camera, component::Transform>()) {
      const auto& targetTransform{ ctx.registry.getComponent<component::Transform>(targetEntity) };
      auto& cameraTransform{ ctx.registry.getComponent<component::Transform>(entity) };
      auto& camera{ ctx.registry.getComponent<component::Camera>(entity) };

      glm::vec2 targetPos{ targetTransform.position };

      bool hasZoomSettings{ false };
      float targetZoom{ 1.f };
      float zoomSpeed{ smoothness };
      float followSpeed{ smoothness };

      if (ctx.registry.hasComponent<component::Player>(targetEntity) &&
          ctx.registry.hasComponent<component::PlayerState>(targetEntity) &&
          ctx.registry.hasComponent<component::PlayerCameraSettings>(targetEntity)) {
        const auto& settings{ ctx.registry.getComponent<component::PlayerCameraSettings>(targetEntity) };
        const auto& playerState{ ctx.registry.getComponent<component::PlayerState>(targetEntity) };

        if (playerState.isAiming && playerState.isSprinting) {
          targetZoom = (settings.aimZoom + settings.sprintZoom) * 0.5f;
          followSpeed = (settings.aimFollowSpeed + settings.baseFollowSpeed) * 0.5f;
        } else if (playerState.isAiming) {
          targetZoom = settings.aimZoom;
          followSpeed = settings.aimFollowSpeed;
        } else if (playerState.isSprinting) {
          targetZoom = settings.sprintZoom;
          followSpeed = settings.baseFollowSpeed;
        } else {
          targetZoom = settings.baseZoom;
          followSpeed = settings.baseFollowSpeed;
        }

        if (playerState.isAiming) {
          glm::vec2 mouseWorldPostion{
            screenToWorld(input_system::getViewportMousePosition(), input_system::getViewportSize(), camera)
          };

          glm::vec2 toCursor{ mouseWorldPostion - targetTransform.position };
          float dist{ glm::length(toCursor) };
          if (dist > 0.001f) {
            float offsetDist{ glm::min(dist * settings.aimOffsetWeight, settings.maxAimOffset) };
            targetPos += glm::normalize(toCursor) * offsetDist;
          }
        }

        zoomSpeed = settings.zoomSpeed;
        hasZoomSettings = true;
      }

      float positionFactor{ glm::clamp(followSpeed * ctx.dt, 0.f, 1.f) };
      cameraTransform.position = glm::mix(cameraTransform.position, targetPos, positionFactor);

      if (hasZoomSettings) {
        float zoomFactor{ glm::clamp(zoomSpeed * ctx.dt, 0.f, 1.f) };
        camera.zoom = glm::mix(camera.zoom, targetZoom, zoomFactor);
      }
    }
  }

  glm::vec2 screenToWorld(glm::vec2 mousePosition, glm::vec2 viewportSize, const component::Camera& camera) {
    if (viewportSize.x <= 0.0f || viewportSize.y <= 0.0f) {
      return glm::vec2(0.0f);
    }

    float ndcX{ (2.0f * mousePosition.x) / viewportSize.x - 1.0f };
    float ndcY{ 1.0f - (2.0f * mousePosition.y) / viewportSize.y };

    glm::vec4 worldPos{ glm::inverse(camera.projection * camera.view) * glm::vec4(ndcX, ndcY, 0.0f, 1.0f) };
    return glm::vec2(worldPos.x, worldPos.y);
  }

  void addImpulse(ecs::Registry& registry, const ecs::EntityId cameraEntity, glm::vec2 impulse) {
    if (registry.hasComponent<component::CameraShake>(cameraEntity)) {
      auto& shake{ registry.getComponent<component::CameraShake>(cameraEntity) };
      shake.velocity += impulse;
    }
  }

  void addTrauma(ecs::Registry& registry, const ecs::EntityId cameraEntity, float amount) {
    if (registry.hasComponent<component::CameraShake>(cameraEntity)) {
      auto& shake{ registry.getComponent<component::CameraShake>(cameraEntity) };
      shake.trauma = std::min(1.f, shake.trauma + amount);
    }
  }

}  // namespace ls::camera_system
