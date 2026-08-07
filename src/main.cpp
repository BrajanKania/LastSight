#include <SDL3/SDL_log.h>

#include <algorithm>

#include "engine/core/scene_manager.hpp"
#include "engine/core/time_system.hpp"
#include "engine/core/window.hpp"
#include "engine/input/input_system.hpp"
#include "engine/ui/ui_system.hpp"
#include "game/scenes/world_scene.hpp"

int main() {
  constexpr int kWidth{ 1500 };
  constexpr int kHeight{ 900 };

  try {
    ls::Window window(kWidth, kHeight);
    ls::ui_system::init(window.getSDLWindow(), window.getOpengGlContext());

    ls::SceneManager sceneManager(window.getWidth(), window.getHeight());
    sceneManager.pushScene<ls::WorldScene>();

    uint64_t lastTime{ ls::time_system::ms() };
    while (!window.shouldClose()) {
      uint64_t currentTime{ ls::time_system::ms() };
      float dt{ static_cast<float>(currentTime - lastTime) / 1000.f };
      dt = std::min(dt, 0.1f);
      lastTime = currentTime;

      window.pollEvents();

      if (ls::input_system::isKeyPressed(ls::input::Key::Escape)) {
        window.close();
      }

      if (window.wasResized()) {
        sceneManager.onResize(window.getWidth(), window.getHeight());
      }

      sceneManager.handleInput();

      sceneManager.update(dt);

      sceneManager.render();

      window.swapBuffers();
    }

    ls::ui_system::shutdown();

  } catch (const std::exception& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[\n%s\n]", e.what());
    return -1;
  }

  return 0;
}
