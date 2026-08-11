#pragma once

namespace ls {

  class Window;
  class SceneManager;

  struct EngineContext {
    Window* window{ nullptr };
    SceneManager* sceneManager{ nullptr };
  };

}  // namespace ls
