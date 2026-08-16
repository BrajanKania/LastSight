#pragma once

#include "engine/core/engine_mode.hpp"

namespace ls {

  class Window;
  class SceneManager;
  namespace dispatch {
    class EventQueue;
  }
  namespace gfx {
    class TextureManager;
  }

  struct EngineContext {
    Window* window{ nullptr };
    SceneManager* sceneManager{ nullptr };
    dispatch::EventQueue* eventQueue{ nullptr };
    gfx::TextureManager* textureManager{ nullptr };
    const EngineMode* engineMode{ nullptr };
  };

}  // namespace ls
