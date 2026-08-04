#pragma once

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/input/input_manager.hpp"

namespace ls {

  struct UpdateContext {
    ecs::Registry& registry;
    dispatch::EventQueue& eventQueue;
    gfx::TextureManager& textureManager;
    input::InputManager& inputManager;
    float dt;
  };

}  // namespace ls
