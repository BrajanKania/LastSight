#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <glad/gl.h>

#include "engine/core/input_system.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/core/window.hpp"
#include "game/scenes/world_scene.hpp"

int main() {
  try {
    ls::Window window(1000, 800);
    ls::SceneManager sceneManager{};

    sceneManager.pushScene<ls::WorldScene>();

    uint64_t lastTime{SDL_GetTicks()};
    while (!window.shouldClose()) {
      uint64_t currentTime{SDL_GetTicks()};
      float dt{static_cast<float>(currentTime - lastTime) / 1000.f};
      lastTime = currentTime;

      window.pollEvents();

      if (ls::input_system::isKeyPressed(ls::input_system::Key::Escape)) {
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

  } catch (const std::exception& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[\n%s\n]", e.what());
    return -1;
  }

  return 0;
}
