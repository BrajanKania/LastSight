#pragma once

#include "engine/core/editor_layer.hpp"
#include "engine/core/engine_context.hpp"
#include "engine/core/engine_mode.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/window.hpp"
#include "engine/debug/console.hpp"
#include "engine/debug/status_bar.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/platform/external_tool_manager.hpp"

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
    void loadTextures();
    void registerConsoleCommands();

    EngineContext getEngineContext() {
      return EngineContext{
        .window = &window_,
        .sceneManager = &sceneManager_,
        .eventQueue = &eventQueue_,
        .textureManager = &textureManager_,
        .engineMode = &engineMode_,
        .console = &console_,
        .statusBar = &statusBar_,
      };
    }

    Window window_;
    input::InputManager inputManager_;
    dispatch::EventQueue eventQueue_;
    gfx::TextureManager textureManager_;
    SceneManager sceneManager_;
    EditorLayer editorLayer_;
    platform::ExternalToolManager externalToolManager_;
    debug::Console console_;
    debug::StatusBar statusBar_;

    EngineMode engineMode_{ EngineMode::Edit };
  };

}  // namespace ls
