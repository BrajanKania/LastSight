#include "engine/core/engine.hpp"

#include <imgui.h>

#include <filesystem>
#include <glm/ext/vector_float4.hpp>
#include <string>

#include "engine/actions/quit_engine.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/time_system.hpp"
#include "engine/core/window.hpp"
#include "engine/debug/command.hpp"
#include "engine/debug/log_level.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/engine_mode_changed.hpp"
#include "engine/events/entity_duplicated.hpp"
#include "engine/events/request_add_component.hpp"
#include "engine/events/request_change_engine_mode.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/events/request_change_ui_style.hpp"
#include "engine/events/request_create_entity.hpp"
#include "engine/events/request_destroy_entity.hpp"
#include "engine/events/request_duplicate_entity.hpp"
#include "engine/events/request_open_asset.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/events/request_reload_textures.hpp"
#include "engine/events/request_remove_component.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/events/viewport_resized.hpp"
#include "engine/input/types.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/renderer/render_system.hpp"
#include "engine/serialization/scene_serializer.hpp"
#include "engine/ui/ui_style.hpp"
#include "engine/ui/ui_system.hpp"

namespace ls {

  Engine::Engine()
      : window_(1500, 900),
        sceneManager_(getEngineContext(), window_.getWidth(), window_.getHeight()) {
    reflection_system::registerGeneratedTypes();

    ui_system::init(window_.getSDLWindow(), window_.getOpengGlContext());

    inputManager_.bindKey<action::QuitEngine>(input::Key::Escape);
    sceneManager_.onResize(window_.getWidth(), window_.getHeight());

    loadTextures();
    registerConsoleCommands();
  }

  Engine::~Engine() { ui_system::shutdown(); }

  void Engine::run() {
    uint64_t lastTime{ ls::time_system::ms() };

    while (!window_.shouldClose()) {
      render_system::bindFramebuffer(0);
      render_system::clearColorBuffer();

      uint64_t currentTime{ ls::time_system::ms() };
      float dt{ static_cast<float>(currentTime - lastTime) / 1000.f };
      dt = std::min(dt, 0.1f);
      lastTime = currentTime;

      window_.pollEvents();

      handleInput();

      sceneManager_.update(dt);
      sceneManager_.render();

      ui_system::beginFrame();

      editorLayer_.render(getEngineContext(), sceneManager_.getActiveSceneContext());
      sceneManager_.renderUI();

      handleRequest();

      editorLayer_.update(getEngineContext());
      ui_system::endFrame();

      window_.swapBuffers();
      eventQueue_.clear();
    }
  }

  void Engine::handleRequest() {
    for (const auto& event : eventQueue_.getEvents<event::RequestQuitEngine>()) {
      window_.close();
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestChangeScene>()) {
      sceneManager_.changeScene(event.name);
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestChangeEngineMode>()) {
      if (engineMode_ != event.newMode) {
        engineMode_ = event.newMode;
        eventQueue_.publish(
            event::EngineModeChanged{
                .newMode = engineMode_,
            }
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::ViewportResized>()) {
      sceneManager_.onResize(event.newSize.x, event.newSize.y);
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestReloadTextures>()) {
      textureManager_.clear();
      loadTextures();
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestOpenAsset>()) {
      externalToolManager_.openInEditor(event.path);
    }

    const auto& sceneCtx{ sceneManager_.getActiveSceneContext() };

    for (const auto& event : eventQueue_.getEvents<event::RequestCreateEntity>()) {
      auto entity{ sceneCtx.registry->createEntity() };
      sceneCtx.registry->addComponent(
          entity,
          component::EntityName{
              .name = event.entityName,
          }
      );
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestDuplicateEntity>()) {
      if (sceneCtx.registry->isValidEntity(event.entityToDuplicate)) {
        ecs::EntityId entity{ sceneCtx.registry->duplicateEntity(event.entityToDuplicate) };
        if (entity != ecs::kNullEntity) {
          if (sceneCtx.registry->hasComponent<component::EntityName>(entity)) {
            auto& name{ sceneCtx.registry->getComponent<component::EntityName>(entity) };
            name.name += "_copy";
          }

          eventQueue_.publish(
              event::EntityDuplicated{
                  .sourceEntity = event.entityToDuplicate,
                  .newEntity = entity,
              }
          );
        }
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestDestroyEntity>()) {
      if (sceneCtx.registry->isValidEntity(event.entity)) {
        sceneCtx.registry->destroyEntity(event.entity);
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestSaveScene>()) {
      serialization::SceneSerializer serializer(sceneCtx, getEngineContext());
      serializer.saveScene(sceneManager_.getActiveSceneName());
    }

    if (sceneCtx.uiManager != nullptr) {
      for (const auto& event : eventQueue_.getEvents<event::TogglePanel>()) {
        if (sceneCtx.uiManager->hasPanel(event.name)) {
          sceneCtx.uiManager->getPanel(event.name).toggleVisible();
        }
      }

      for (const auto& event : eventQueue_.getEvents<event::SetPanelVisibility>()) {
        if (sceneCtx.uiManager->hasPanel(event.name)) {
          sceneCtx.uiManager->getPanel(event.name).setVisible(event.visible);
        }
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestAddComponent>()) {
      if (!sceneCtx.registry->isValidEntity(event.entity))
        continue;

      auto* set{ sceneCtx.registry->getISparseSetByTypeId(event.typeId) };
      if (set) {
        set->addComponent(event.entity);
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestRemoveComponent>()) {
      auto* set{ sceneCtx.registry->getISparseSetByTypeId(event.typeId) };
      if (set) {
        set->destroyComponent(event.entity);
      }
    }
  }

  void Engine::handleInput() {
    ImGuiIO& io{ ImGui::GetIO() };

    const bool isPlayMode{ engineMode_ == EngineMode::Play };
    const bool blockKeyboard = !isPlayMode && io.WantCaptureKeyboard && !editorLayer_.isViewportFocused();
    const bool blockMouse = !isPlayMode && io.WantCaptureMouse && !editorLayer_.isViewportHovered();

    editorLayer_.handleInput(getEngineContext(), blockKeyboard, blockMouse);
    inputManager_.update(blockKeyboard, blockMouse);
    sceneManager_.handleInput(blockKeyboard, blockMouse);

    auto actionQuitEngineState{ inputManager_.getActionState<action::QuitEngine>() };
    if (actionQuitEngineState == input::ActionState::JustPressed) {
      eventQueue_.publish(event::RequestQuitEngine{});
    }
  }

  void Engine::loadTextures() {
    textureManager_.loadFromDir(std::filesystem::path("src/engine/assets/textures"));
    textureManager_.loadFromDir(std::filesystem::path("assets/textures"));
  }

  void Engine::registerConsoleCommands() {
    console_.registerCommand("clear", "clear", [this](debug::CommandArgs) { console_.clear(); });
    console_.registerCommand("exit", "exit", [this](debug::CommandArgs) {
      eventQueue_.publish(event::RequestQuitEngine{});
    });
    console_.registerCommand("set_ui_style", "set_ui_style <int>", [this](debug::CommandArgs args) {
      if (args.size() == 0) {
        return;
      }

      try {
        int style{ std::stoi(args[0]) };
        eventQueue_.publish(event::RequestChangeUIStyle{ .newStyle = static_cast<ui::UIStyle>(style) });
      } catch (...) {
        console_.log("Invalid arguments: set_ui_style <int>", debug::LogLevel::Error);
        return;
      }
    });
    console_.registerCommand("help", "help", [this](debug::CommandArgs) {
      console_.log("Command list:", debug::LogLevel::Info);
      const auto& commands{ console_.getCommands() };
      for (const auto& command : commands) {
        console_.log(std::format(" {}", command.second.description), debug::LogLevel::Info);
      }
    });
  }

}  // namespace ls
