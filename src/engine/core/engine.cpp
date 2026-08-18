#include "engine/core/engine.hpp"

#include <imgui.h>

#include <filesystem>
#include <glm/ext/vector_float4.hpp>

#include "engine/actions/quit_engine.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/time_system.hpp"
#include "engine/core/window.hpp"
#include "engine/events/engine_mode_changed.hpp"
#include "engine/events/request_change_engine_mode.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/events/request_reload_textures.hpp"
#include "engine/events/viewport_resized.hpp"
#include "engine/input/types.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/renderer/render_system.hpp"
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

}  // namespace ls
