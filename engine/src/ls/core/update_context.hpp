#pragma once

#include "ls/dispatch/event_queue.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/gfx/texture_manager.hpp"
#include "ls/input/input_manager.hpp"
#include "ls/renderer/material/material_manager.hpp"

namespace ls {

  struct UpdateContext {
    ecs::Registry& registry;
    dispatch::EventQueue& eventQueue;
    renderer::MaterialManager& materialManager;
    gfx::TextureManager& textureManager;
    input::InputManager& inputManager;
    float dt;
  };

}  // namespace ls
