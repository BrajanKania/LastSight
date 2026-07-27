#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <glad/gl.h>

#include <glm/ext/matrix_transform.hpp>

#include "core/controller_system.hpp"
#include "core/scene_manager.hpp"
#include "core/window.hpp"
#include "renderer/renderer_system.hpp"
#include "scene/world_scene.hpp"

int main() {
  try {
    ls::Window window(1000, 800);
    ls::SceneManager sceneManager{};

    sceneManager.pushScene<ls::WorldScene>();

    ls::renderer_system::setClearColor({0.2f, 0.2f, 0.2f, 1.f});

    uint64_t lastTime{SDL_GetTicks()};
    while (!window.shouldClose()) {
      uint64_t currentTime{SDL_GetTicks()};
      float dt{static_cast<float>(currentTime - lastTime) / 1000.f};
      lastTime = currentTime;

      window.pollEvents();

      if (ls::controller_system::isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        window.close();
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
