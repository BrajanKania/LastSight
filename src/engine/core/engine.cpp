#include "engine/core/engine.hpp"

#include <imgui.h>

#include <filesystem>
#include <format>
#include <glm/ext/vector_float4.hpp>
#include <string>

#include "engine/actions/toggle_engine_mode.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/components/prefab_instance.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/core/engine_mode.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/time_system.hpp"
#include "engine/core/window.hpp"
#include "engine/debug/command.hpp"
#include "engine/debug/log_level.hpp"
#include "engine/debug/status_notification.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/engine_mode_changed.hpp"
#include "engine/events/entity_duplicated.hpp"
#include "engine/events/request_add_component.hpp"
#include "engine/events/request_change_engine_mode.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/events/request_change_ui_style.hpp"
#include "engine/events/request_create_entity.hpp"
#include "engine/events/request_create_entity_from_prefab.hpp"
#include "engine/events/request_decouple_entity_from_prefab.hpp"
#include "engine/events/request_destroy_entity.hpp"
#include "engine/events/request_duplicate_entity.hpp"
#include "engine/events/request_link_entity_to_prefab.hpp"
#include "engine/events/request_open_asset.hpp"
#include "engine/events/request_overwrite_entity_prefab.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/events/request_reload_textures.hpp"
#include "engine/events/request_remove_component.hpp"
#include "engine/events/request_save_entity_as_prefab.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/events/request_sync_component_from_prefab.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/events/viewport_resized.hpp"
#include "engine/input/input_context.hpp"
#include "engine/input/types.hpp"
#include "engine/prefab/prefab_manager.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/renderer/render_system.hpp"
#include "engine/serialization/scene_serializer.hpp"
#include "engine/ui/ui_style.hpp"
#include "engine/ui/ui_system.hpp"

namespace ls {

  Engine::Engine()
      : window_(1500, 900),
        prefabManager_(getPrefabContext()),
        sceneManager_(getEngineContext(), window_.getWidth(), window_.getHeight()) {
    reflection_system::registerGeneratedTypes();

    ui_system::init(window_.getSDLWindow(), window_.getOpengGlContext());

    inputManager_.bindKey<action::ToggleEngineMode>(input::Key::Grave, input::KeyModifier::Ctrl);

    sceneManager_.onResize(window_.getWidth(), window_.getHeight());

    loadTextures();
    loadPrefabs();
    registerConsoleCommands();

    eventQueue_.publish(event::EngineModeChanged{});
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

      statusBar_.update(dt);

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
      statusBar_.showMessage("Reloaded textures", debug::status_duration::kMedium, debug::LogLevel::Info);
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
          std::string entityName{ "Entity" };
          if (sceneCtx.registry->hasComponent<component::EntityName>(entity)) {
            auto& name{ sceneCtx.registry->getComponent<component::EntityName>(entity) };
            name.name += "_copy";
            entityName = name.name;
          }

          eventQueue_.publish(
              event::EntityDuplicated{
                  .sourceEntity = event.entityToDuplicate,
                  .newEntity = entity,
              }
          );

          statusBar_.showMessage(
              std::format("Duplicated entity: {}", entityName), debug::status_duration::kMedium, debug::LogLevel::Info
          );
        }
      } else {
        statusBar_.showMessage(
            "Cannot duplicate: No valid entity selected", debug::status_duration::kMedium, debug::LogLevel::Warning
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestDestroyEntity>()) {
      if (sceneCtx.registry->isValidEntity(event.entity)) {
        sceneCtx.registry->destroyEntity(event.entity);
        statusBar_.showMessage("Entity destroyed", debug::status_duration::kShort, debug::LogLevel::Info);
      } else {
        statusBar_.showMessage(
            "Cannot destroy: Non valid entity selected", debug::status_duration::kMedium, debug::LogLevel::Warning
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestSaveScene>()) {
      serialization::SceneSerializer serializer(sceneCtx, getEngineContext());
      const auto& path{ asset_system::scene(sceneManager_.getActiveSceneName()) };
      if (serializer.saveScene(path)) {
        statusBar_.showMessage(
            std::format("Saved scene: {}", path.string()), debug::status_duration::kLong, debug::LogLevel::Info
        );
      } else {
        statusBar_.showMessage(
            std::format("Failed to save scene: {}", path.string()),
            debug::status_duration::kLong,
            debug::LogLevel::Error
        );
      }
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

    for (const auto& event : eventQueue_.getEvents<event::RequestSaveEntityAsPrefab>()) {
      if (prefabManager_.save(event.name, *sceneCtx.registry, event.entity)) {
        statusBar_.showMessage(
            std::format("Saved as prefab: {}", asset_system::prefab(event.name).string()),
            debug::status_duration::kLong,
            debug::LogLevel::Info
        );
      } else {
        statusBar_.showMessage(
            std::format("Cannot save as prefab: {}", event.name),
            debug::status_duration::kMedium,
            debug::LogLevel::Warning
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestOverwriteEntityPrefab>()) {
      auto prefabInfo{ prefabManager_.getPrefabInfo(event.handle) };

      if (prefabInfo && prefabManager_.save(prefabInfo->name, *sceneCtx.registry, event.entity)) {
        statusBar_.showMessage(
            std::format("Overwrote prefab: {}", asset_system::prefab(prefabInfo->name).string()),
            debug::status_duration::kLong,
            debug::LogLevel::Info
        );
      } else {
        statusBar_.showMessage(
            "Failed to overwrite prefab: Invalid prefab", debug::status_duration::kMedium, debug::LogLevel::Warning
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestCreateEntityFromPrefab>()) {
      if (event.handle.isValid()) {
        auto prefabInfo{ prefabManager_.getPrefabInfo(event.handle) };
        auto newEntity{ prefabManager_.instantiate(event.handle, *sceneCtx.registry) };
        if (newEntity != ecs::kNullEntity) {
          statusBar_.showMessage(
              std::format("Instantiate {}", prefabInfo ? prefabInfo->name : "Invalid prefab"),
              debug::status_duration::kMedium,
              debug::LogLevel::Info
          );
        } else {
          statusBar_.showMessage(
              std::format("Cannot instantiate: {}", prefabInfo ? prefabInfo->name : "Invalid prefab"),
              debug::status_duration::kMedium,
              debug::LogLevel::Warning
          );
        }
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestDecoupleEntityFromPrefab>()) {
      if (sceneCtx.registry->isValidEntity(event.entity) &&
          sceneCtx.registry->hasComponent<component::PrefabInstance>(event.entity)) {
        sceneCtx.registry->destroyComponent<component::PrefabInstance>(event.entity);
        statusBar_.showMessage("Decoupled entity from prefab", debug::status_duration::kMedium, debug::LogLevel::Info);
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestLinkEntityToPrefab>()) {
      if (sceneCtx.registry->isValidEntity(event.entity)) {
        auto prefabInfo{ prefabManager_.getPrefabInfo(event.handle) };
        if (prefabInfo) {
          if (sceneCtx.registry->hasComponent<component::PrefabInstance>(event.entity)) {
            sceneCtx.registry->getComponent<component::PrefabInstance>(event.entity).handle = event.handle;
          } else {
            sceneCtx.registry->addComponent(event.entity, component::PrefabInstance{ .handle = event.handle });
          }
          statusBar_.showMessage(
              std::format("Link entity to prefab: {}", prefabInfo->name),
              debug::status_duration::kMedium,
              debug::LogLevel::Info
          );
        }
      } else {
        statusBar_.showMessage(
            "Cannot link entity to prefab: Invalid prefab", debug::status_duration::kMedium, debug::LogLevel::Warning
        );
      }
    }

    for (const auto& event : eventQueue_.getEvents<event::RequestSyncComponentFromPrefab>()) {
      if (sceneCtx.registry->isValidEntity(event.entity) &&
          sceneCtx.registry->hasComponent<component::PrefabInstance>(event.entity)) {
        auto prefabInstance{ sceneCtx.registry->getComponent<component::PrefabInstance>(event.entity) };
        auto prefabInfo{ prefabManager_.getPrefabInfo(prefabInstance.handle) };
        if (prefabInfo &&
            prefabManager_.syncComponent(prefabInstance.handle, event.typeId, *sceneCtx.registry, event.entity)) {
          statusBar_.showMessage(
              std::format("Sync entity from prefab: {}", prefabInfo->name),
              debug::status_duration::kMedium,
              debug::LogLevel::Info
          );
        } else {
          statusBar_.showMessage(
              "Failed to sync entity from prefab: Invalid prefab",
              debug::status_duration::kMedium,
              debug::LogLevel::Warning
          );
        }
      }
    }
  }

  void Engine::handleInput() {
    ImGuiIO& io{ ImGui::GetIO() };
    const bool isEditorMode{ engineMode_ == EngineMode::Edit };

    input::ConsumedInputState consumedInputState{};

    input::InputContext engineInputCtx{
      .captureKeyboard = false,
      .captureMouse = false,
      .consumedInputState = consumedInputState,
    };
    inputManager_.update(engineInputCtx);

    if (isEditorMode) {
      input::InputContext editorInputCtx{
        .captureKeyboard = io.WantTextInput,
        .captureMouse = false,
        .consumedInputState = consumedInputState,
      };
      editorLayer_.handleInput(getEngineContext(), editorInputCtx);
    }

    const bool blockKeyboardForScene{ isEditorMode && io.WantCaptureKeyboard && !editorLayer_.isViewportFocused() };
    const bool blockMouseForScene{ isEditorMode && io.WantCaptureMouse && !editorLayer_.isViewportHovered() };

    input::InputContext sceneInputCtx{
      .captureKeyboard = blockKeyboardForScene,
      .captureMouse = blockMouseForScene,
      .consumedInputState = consumedInputState,
    };
    sceneManager_.handleInput(sceneInputCtx);

    auto toggleEngineModeActionState{ inputManager_.getActionState<action::ToggleEngineMode>() };
    if (toggleEngineModeActionState == input::ActionState::JustPressed) {
      eventQueue_.publish(
          event::RequestChangeEngineMode{
              .newMode = (engineMode_ == EngineMode::Edit ? EngineMode::Play : EngineMode::Edit),
          }
      );
    }
  }

  void Engine::loadTextures() {
    textureManager_.loadFromDir(std::filesystem::path("src/engine/assets/textures"));
    textureManager_.loadFromDir(std::filesystem::path("assets/textures"));
  }

  void Engine::loadPrefabs() { prefabManager_.loadFromDir(std::filesystem::path("assets/prefabs")); }

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
