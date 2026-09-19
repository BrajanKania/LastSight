#pragma once

#include "ls/asset/asset_context.hpp"
#include "ls/asset/asset_manager.hpp"
#include "ls/core/editor_layer.hpp"
#include "ls/core/engine_context.hpp"
#include "ls/core/engine_mode.hpp"
#include "ls/core/scene_manager.hpp"
#include "ls/core/window.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/status_bar.hpp"
#include "ls/dispatch/event_queue.hpp"
#include "ls/gfx/texture_manager.hpp"
#include "ls/input/input_manager.hpp"
#include "ls/platform/external_tool_manager.hpp"
#include "ls/prefab/prefab_context.hpp"
#include "ls/prefab/prefab_manager.hpp"
#include "ls/renderer/material/material_manager.hpp"
#include "ls/renderer/render_pipeline.hpp"

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
    void loadPrefabs();
    void loadMaterials();
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
        .materialManager = &materialManager_,
        .renderPipeline = &renderPipeline_,
        .prefabManager = &prefabManager_,
        .assetManager = &assetManager_,
      };
    }

    prefab::PrefabContext getPrefabContext() {
      return prefab::PrefabContext{
        .materialManager = &materialManager_,
        .console = &console_,
      };
    }

    asset::AssetContext getAssetContext() {
      return asset::AssetContext{
        .console = &console_,
      };
    }

    Window window_;
    input::InputManager inputManager_{};
    dispatch::EventQueue eventQueue_{};
    gfx::TextureManager textureManager_{};
    debug::Console console_{};
    debug::StatusBar statusBar_{};
    asset::AssetManager assetManager_;
    renderer::MaterialManager materialManager_{};
    renderer::RenderPipeline renderPipeline_{};
    prefab::PrefabManager prefabManager_;
    SceneManager sceneManager_;
    EditorLayer editorLayer_{};
    platform::ExternalToolManager externalToolManager_{};

    EngineMode engineMode_{ EngineMode::Edit };
  };

}  // namespace ls
