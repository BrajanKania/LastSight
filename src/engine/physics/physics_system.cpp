#include "engine/physics/physics_system.hpp"

#include <glm/ext/quaternion_geometric.hpp>
#include <glm/glm.hpp>

#include "engine/components/collider.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/events/collision.hpp"

namespace ls::physics_system {

  namespace {

    void applyVelocity(ecs::Registry& registry, float dt) {
      for (auto entity : registry.view<component::Transform, component::Velocity>()) {
        auto& transform{ registry.getComponent<component::Transform>(entity) };
        const auto& velocity{ registry.getComponent<component::Velocity>(entity) };

        transform.position += velocity.linear * dt;
        transform.rotation += velocity.angular * dt;
      }
    }

    void resolveCollision(ecs::Registry& registry, const event::Collision& collision) {
      auto& tA{ registry.getComponent<component::Transform>(collision.entityA) };
      auto& tB{ registry.getComponent<component::Transform>(collision.entityB) };

      glm::vec2 separation{ collision.normal * collision.depth };
      tA.position -= separation * 0.5f;
      tB.position += separation * 0.5f;
    }

    bool checkCircleVsCircle(
        const component::Transform& tA,
        const component::Collider& cA,
        const component::Transform& tB,
        const component::Collider& cB,
        event::Collision& col
    ) {
      glm::vec2 posA{ tA.position + cA.offset };
      glm::vec2 posB{ tB.position + cB.offset };

      float rA{ cA.radius * tA.scale.x };
      float rB{ cB.radius * tB.scale.x };

      glm::vec2 dist{ posA - posB };
      float distLen{ glm::length(dist) };
      float radiusSum{ rA + rB };

      if (distLen >= radiusSum || distLen == 0.0f)
        return false;

      col.isTrigger = (cA.isTrigger || cB.isTrigger);
      col.depth = radiusSum - distLen;
      col.normal = -glm::normalize(dist);

      return true;
    }

    bool checkOBBVsCircle(
        const component::Transform& tBox,
        const component::Collider& cBox,
        const component::Transform& tCircle,
        const component::Collider& cCircle,
        event::Collision& col,
        bool isAbox
    ) {
      glm::vec2 boxPos{ tBox.position + cBox.offset };
      glm::vec2 circlePos{ tCircle.position + cCircle.offset };
      float radius{ cCircle.radius * tCircle.scale.x };
      glm::vec2 halfExtents{ cBox.halfExtents * tBox.scale };

      float rad{ (cBox.type == component::ColliderType::AABB) ? 0.0f : glm::radians(tBox.rotation) };
      float cosR{ std::cos(-rad) };
      float sinR{ std::sin(-rad) };

      glm::vec2 relPos{ circlePos - boxPos };
      glm::vec2 localCirclePos{ relPos.x * cosR - relPos.y * sinR, relPos.x * sinR + relPos.y * cosR };

      glm::vec2 closestLocal{ glm::clamp(localCirclePos, -halfExtents, halfExtents) };
      glm::vec2 localDist{ localCirclePos - closestLocal };
      float distLen{ glm::length(localDist) };

      if (distLen >= radius)
        return false;

      glm::vec2 localNormal{ 0.f };

      if (distLen == 0.0f) {
        glm::vec2 absLocal{ glm::abs(localCirclePos) };
        glm::vec2 diff{ halfExtents - absLocal };

        if (diff.x < diff.y) {
          localNormal = glm::vec2(localCirclePos.x < 0.f ? -1.f : 1.f, 0.f);
          distLen = -diff.x;
        } else {
          localNormal = glm::vec2(0.f, localCirclePos.y < 0.f ? -1.f : 1.f);
          distLen = -diff.y;
        }
      } else {
        localNormal = glm::normalize(localDist);
      }

      cosR = std::cos(rad);
      sinR = std::sin(rad);

      glm::vec2 worldNormal{ localNormal.x * cosR - localNormal.y * sinR, localNormal.x * sinR + localNormal.y * cosR };

      col.isTrigger = (cBox.isTrigger || cCircle.isTrigger);
      col.depth = radius - distLen;
      col.normal = isAbox ? worldNormal : -worldNormal;

      return true;
    }

    bool checkOBBVsOBB(
        const component::Transform& tA,
        const component::Collider& cA,
        const component::Transform& tB,
        const component::Collider& cB,
        event::Collision& col
    ) {
      auto getAxes{
        [](const component::Transform& t, const component::Collider& c, glm::vec2 axes[2], glm::vec2& half) {
          float rad{ (c.type == component::ColliderType::AABB) ? 0.0f : glm::radians(t.rotation) };
          axes[0] = glm::vec2(std::cos(rad), std::sin(rad));
          axes[1] = glm::vec2(-std::sin(rad), std::cos(rad));
          half = c.halfExtents * t.scale;
        }
      };

      glm::vec2 axesA[2], axesB[2];
      glm::vec2 halfA, halfB;
      getAxes(tA, cA, axesA, halfA);
      getAxes(tB, cB, axesB, halfB);

      glm::vec2 posA{ tA.position + cA.offset };
      glm::vec2 posB{ tB.position + cB.offset };
      glm::vec2 dist{ posA - posB };

      float minOverlap{ 1e9f };
      glm::vec2 bestAxis{ 0.0f };

      glm::vec2 testAxes[4]{ axesA[0], axesA[1], axesB[0], axesB[1] };

      for (int i{ 0 }; i < 4; ++i) {
        glm::vec2 axis{ testAxes[i] };

        float projA{ halfA.x * std::abs(glm::dot(axesA[0], axis)) + halfA.y * std::abs(glm::dot(axesA[1], axis)) };
        float projB{ halfB.x * std::abs(glm::dot(axesB[0], axis)) + halfB.y * std::abs(glm::dot(axesB[1], axis)) };

        float distanceProj{ std::abs(glm::dot(dist, axis)) };
        float overlap{ (projA + projB) - distanceProj };

        if (overlap <= 0.0f)
          return false;

        if (overlap < minOverlap) {
          minOverlap = overlap;
          bestAxis = axis;
        }
      }

      col.isTrigger = (cA.isTrigger || cB.isTrigger);
      col.depth = minOverlap;
      col.normal = (glm::dot(dist, bestAxis) < 0.0f) ? bestAxis : -bestAxis;

      return true;
    }

    void resolveCollisions(ecs::Registry& registry) {
      event::Collision collision;
      auto view{ registry.view<component::Collider, component::Transform>() };

      for (auto itA{ view.begin() }; itA != view.end(); ++itA) {
        for (auto itB{ std::next(itA) }; itB != view.end(); ++itB) {
          auto entityA{ *itA };
          auto entityB{ *itB };

          const auto& transA{ registry.getComponent<component::Transform>(entityA) };
          const auto& colA{ registry.getComponent<component::Collider>(entityA) };
          const auto& transB{ registry.getComponent<component::Transform>(entityB) };
          const auto& colB{ registry.getComponent<component::Collider>(entityB) };

          bool wasCollision{ false };
          if (colA.type == component::ColliderType::Circle && colB.type == component::ColliderType::Circle) {
            wasCollision = checkCircleVsCircle(transA, colA, transB, colB, collision);
          } else if (
              colA.type == component::ColliderType::Circle &&
              (colB.type == component::ColliderType::AABB || colB.type == component::ColliderType::OBB)
          ) {
            wasCollision = checkOBBVsCircle(transB, colB, transA, colA, collision, false);
          } else if (
              (colA.type == component::ColliderType::AABB || colA.type == component::ColliderType::OBB) &&
              colB.type == component::ColliderType::Circle
          ) {
            wasCollision = checkOBBVsCircle(transA, colA, transB, colB, collision, true);
          } else if (
              (colA.type == component::ColliderType::AABB || colA.type == component::ColliderType::OBB) &&
              (colB.type == component::ColliderType::AABB || colB.type == component::ColliderType::OBB)
          ) {
            wasCollision = checkOBBVsOBB(transA, colA, transB, colB, collision);
          }

          if (wasCollision) {
            collision.entityA = entityA;
            collision.entityB = entityB;

            if (!collision.isTrigger) {
              resolveCollision(registry, collision);
            }
          }
        }
      }
    }

  }  // namespace

  void update(ecs::Registry& registry, float dt) {
    applyVelocity(registry, dt);
    resolveCollisions(registry);
  }

}  // namespace ls::physics_system
