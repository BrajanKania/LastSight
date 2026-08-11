#pragma once

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"

namespace ls::dispatch {
  class EventQueue;
}

namespace ls::ui {

  struct UIContext {
    dispatch::EventQueue& eventQueue;
    EngineContext engineCtx;
    SceneContext sceneCtx;
  };

}  // namespace ls::ui
