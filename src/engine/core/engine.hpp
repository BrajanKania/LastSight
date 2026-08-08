#pragma once

#include "engine/core/scene_manager.hpp"
#include "engine/core/window.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/ui/ui_manager.hpp"

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

    Window window_;
    SceneManager sceneManager_;
    input::InputManager inputManager_;
    dispatch::EventQueue eventQueue_;
    ui::UIManager uiManager_;
  };

}  // namespace ls
