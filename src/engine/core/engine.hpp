#pragma once

#include "engine/core/editor_layer.hpp"
#include "engine/core/engine_context.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/window.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/input/input_manager.hpp"

namespace ls {

  class Engine {
  public:
    Engine();
    ~Engine();

    void run();
    SceneManager& getSceneManager() { return sceneManager_; }

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

  private:
    void handleRequest();
    void handleInput();

    EngineContext getEngineContext() {
      return EngineContext{
        .window = &window_,
        .sceneManager = &sceneManager_,
      };
    }

    Window window_;
    SceneManager sceneManager_;
    input::InputManager inputManager_;
    dispatch::EventQueue eventQueue_;
    EditorLayer editorLayer_;
  };

}  // namespace ls
