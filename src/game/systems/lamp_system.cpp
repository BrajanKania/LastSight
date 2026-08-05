#include "game/systems/lamp_system.hpp"

#include "engine/components/particle_emitter.hpp"
#include "engine/core/update_context.hpp"
#include "engine/events/interact_with_entity.hpp"
#include "game/components/lamp.hpp"

namespace ls::lamp_system {

  void update(const UpdateContext& ctx) {
    for (auto event : ctx.eventQueue.getEvents<event::InteractWithEntity>()) {
      if (ctx.registry.hasComponent<component::Lamp>(event.target)) {
        auto& lamp{ ctx.registry.getComponent<component::Lamp>(event.target) };
        lamp.isOn = !lamp.isOn;
      }
    }

    for (auto entity : ctx.registry.view<component::Lamp, component::ParticleEmitter>()) {
      const auto& lamp{ ctx.registry.getComponent<component::Lamp>(entity) };
      auto& emitter{ ctx.registry.getComponent<component::ParticleEmitter>(entity) };
      emitter.isPaused = !lamp.isOn;
    }
  }

}  // namespace ls::lamp_system
