#pragma once

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
  };

}  // namespace ls
