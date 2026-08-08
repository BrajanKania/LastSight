#pragma once

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/gfx/texture_manager.hpp"

namespace ls::ui {

  struct UIContext {
    dispatch::EventQueue& eventQueue;
    ecs::Registry* registry{ nullptr };
    gfx::TextureManager* textureManager{ nullptr };
  };

}  // namespace ls::ui
