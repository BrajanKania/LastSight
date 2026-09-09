#include "systems/fov_system.hpp"

#include <glm/common.hpp>
#include <glm/ext/scalar_common.hpp>

#include "components/field_of_view.hpp"
#include "components/player.hpp"
#include "components/player_fov_settings.hpp"
#include "components/player_state.hpp"
#include "components/stamina.hpp"
#include "ls/ecs/types.hpp"

namespace ls::fov_system {

  void update(const UpdateContext& ctx) {
    for (ecs::EntityId entity : ctx.registry.view<
                                component::Player,
                                component::PlayerState,
                                component::FieldOfView,
                                component::PlayerFovSettings,
                                component::Stamina>()) {
      const auto& playerState{ ctx.registry.getComponent<component::PlayerState>(entity) };
      auto& fov{ ctx.registry.getComponent<component::FieldOfView>(entity) };
      const auto& settings{ ctx.registry.getComponent<component::PlayerFovSettings>(entity) };
      const auto& stamina{ ctx.registry.getComponent<component::Stamina>(entity) };

      float targetAngle{ settings.baseAngle };
      float targetSmoothnessAngle{ settings.baseSmoothnessAngle };
      float targetDarkness{ settings.baseDarkness };

      if (playerState.isAiming) {
        targetAngle = std::min(targetAngle, settings.aimAngle);
        targetSmoothnessAngle = std::min(targetSmoothnessAngle, settings.aimSmoothnessAngle);
        targetDarkness = std::min(targetDarkness, settings.aimDarkness);
      }

      float staminaRatio{ stamina.max > 0.f ? stamina.current / stamina.max : 1.f };
      if (staminaRatio < settings.lowStaminaThreshold) {
        float factor{ staminaRatio / settings.lowStaminaThreshold };
        float staminaTarget{ glm::mix(settings.lowStaminaAngle, settings.baseAngle, factor) };
        targetAngle = std::min(targetAngle, staminaTarget);

        float staminaSmoothnessTarget{
          glm::mix(settings.lowStaminaSmoothnessAngle, settings.baseSmoothnessAngle, factor)
        };
        targetSmoothnessAngle = std::min(targetSmoothnessAngle, staminaSmoothnessTarget);
      }

      fov.fovAngle = glm::mix(fov.fovAngle, targetAngle, settings.transitionSpeed * ctx.dt);
      fov.smoothnessAngle = glm::mix(fov.smoothnessAngle, targetSmoothnessAngle, settings.transitionSpeed * ctx.dt);
      fov.darkness = glm::mix(fov.darkness, targetDarkness, settings.transitionSpeed * ctx.dt);
    }
  }

}  // namespace ls::fov_system
