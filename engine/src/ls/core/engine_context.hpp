#pragma once

#include "ls/core/engine_mode.hpp"

namespace ls {

  class Window;
  class SceneManager;
  namespace dispatch {
    class EventQueue;
  }
  namespace gfx {
    class TextureManager;
  }
  namespace debug {
    class Console;
    class StatusBar;
  }  // namespace debug
  namespace prefab {
    class PrefabManager;
  }
  namespace renderer {
    class MaterialManager;
    class RenderPipeline;
  }  // namespace renderer

  struct EngineContext {
    Window* window{ nullptr };
    SceneManager* sceneManager{ nullptr };
    dispatch::EventQueue* eventQueue{ nullptr };
    gfx::TextureManager* textureManager{ nullptr };
    const EngineMode* engineMode{ nullptr };
    debug::Console* console{ nullptr };
    debug::StatusBar* statusBar{ nullptr };
    renderer::MaterialManager* materialManager{ nullptr };
    renderer::RenderPipeline* renderPipeline{ nullptr };
    prefab::PrefabManager* prefabManager{ nullptr };
  };

}  // namespace ls
