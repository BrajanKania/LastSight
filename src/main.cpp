#include <exception>
#include <format>
#include <iostream>

#include "engine/core/engine.hpp"
#include "game/scenes/main_menu_scene.hpp"
#include "game/scenes/scene_names.hpp"
#include "game/scenes/world_scene.hpp"

int main() {
  try {
    ls::Engine engine{};
    engine.getSceneManager().registerScene<ls::MainMenuScene>(ls::scene::kMainMenu);
    engine.getSceneManager().registerScene<ls::WorldScene>(ls::scene::kWorld);
    engine.getSceneManager().changeScene(ls::scene::kMainMenu);
    engine.run();
  } catch (const std::exception& e) {
    std::cerr << std::format("[\n{}\n]", e.what());
    return -1;
  }

  return 0;
}
