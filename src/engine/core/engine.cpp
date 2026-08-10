#include "engine/core/engine.hpp"

#include <imgui.h>

#include "engine/actions/quit_engine.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/time_system.hpp"
#include "engine/core/window.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/input/types.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/ui/panels/scene_selector_panel.hpp"
#include "engine/ui/ui_context.hpp"
#include "engine/ui/ui_system.hpp"

namespace ls {

  Engine::Engine()
      : window_(1500, 900),
        sceneManager_(window_.getWidth(), window_.getHeight()) {
    reflection::registerGeneratedTypes();

    ui_system::init(window_.getSDLWindow(), window_.getOpengGlContext());

    inputManager_.bindKey<action::QuitEngine>(input::Key::Escape);

    uiManager_.addPanel<ui::SceneSelectorPanel>("scene_selector_panel", sceneManager_);
    uiManager_.getPanel("scene_selector_panel").setVisible(true);
  }

  Engine::~Engine() { ui_system::shutdown(); }

  void Engine::run() {
    uint64_t lastTime{ ls::time_system::ms() };
    while (!window_.shouldClose()) {
      uint64_t currentTime{ ls::time_system::ms() };
      float dt{ static_cast<float>(currentTime - lastTime) / 1000.f };
      dt = std::min(dt, 0.1f);
      lastTime = currentTime;

      window_.pollEvents();

      handleInput();

      if (window_.wasResized()) {
        sceneManager_.onResize(window_.getWidth(), window_.getHeight());
      }

      sceneManager_.handleInput();

      sceneManager_.update(dt);

      ui_system::beginFrame();

      sceneManager_.render();

      ui::UIContext engineUIContenxt{ .eventQueue = eventQueue_ };
      uiManager_.render(engineUIContenxt);

      ui_system::endFrame();

      handleRequest();

      window_.swapBuffers();
      eventQueue_.clear();
    }
  }

  void Engine::handleRequest() {
    for (auto event : eventQueue_.getEvents<event::RequestQuitEngine>()) {
      window_.close();
    }

    for (auto event : eventQueue_.getEvents<event::RequestChangeScene>()) {
      sceneManager_.changeScene(event.name);
    }
  }

  void Engine::handleInput() {
    ImGuiIO& io{ ImGui::GetIO() };
    inputManager_.update(io.WantCaptureKeyboard, io.WantCaptureMouse);

    auto actionQuitEngineState{ inputManager_.getActionState<action::QuitEngine>() };
    if (actionQuitEngineState == input::ActionState::JustPressed) {
      eventQueue_.publish(event::RequestQuitEngine{});
    }
  }

}  // namespace ls
