#pragma once

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/renderer/texture_manager.hpp"

namespace ls::ui {

  struct UIContext {
    ecs::Registry& registry;
    dispatch::EventQueue& eventQueue;
    TextureManager& textureManager;
  };

}  // namespace ls::ui
