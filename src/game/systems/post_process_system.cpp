#include "game/systems/post_process_system.hpp"

#include "engine/ecs/types.hpp"
#include "game/components/health.hpp"
#include "game/components/player.hpp"
#include "game/components/post_process_settings.hpp"
#include "game/components/stamina.hpp"

namespace ls::post_process_system {

  void update(const UpdateContext& ctx) {
    for (ecs::EntityId entity :
         ctx.registry
             .view<component::Player, component::PostProcessSettings, component::Health, component::Stamina>()) {
      auto& settings{ ctx.registry.getComponent<component::PostProcessSettings>(entity) };
      const auto& health{ ctx.registry.getComponent<component::Health>(entity) };
      const auto& stamina{ ctx.registry.getComponent<component::Stamina>(entity) };

      float healthRatio{ health.max > 0.f ? glm::clamp(health.current / health.max, 0.f, 1.f) : 1.f };
      settings.currentDamageIntensity = std::pow(1.f - healthRatio, 3.f);

      float staminaRatio{ stamina.max > 0.f ? glm::clamp(stamina.current / stamina.max, 0.f, 1.f) : 1.f };
      settings.currentStaminaIntensity = std::pow(1.f - staminaRatio, 2.5f);
    }
  }

}  // namespace ls::post_process_system
