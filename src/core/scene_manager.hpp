#pragma once

#include <functional>
#include <memory>

#include "scene.hpp"

namespace ls {

class SceneManager {
public:
  template <typename T>
  void pushScene() {
    pendingOperations_.push_back([this]() {
      auto newScene{std::make_unique<T>()};
      newScene->onEnter();
      scenes_.push_back(std::move(newScene));
    });
  }

  void popScene();

  template <typename T>
  void changeScene() {
    pendingOperations_.push_back([this]() {
      while (!scenes_.empty()) {
        scenes_.back()->onExit();
        scenes_.pop_back();
      }

      auto newScene{std::make_unique<T>()};
      newScene->onEnter();
      scenes_.push_back(std::move(newScene));
    });
  }

  void handleInput();
  void update(float dt);
  void render();

private:
  void processPendingOperations();

  std::vector<std::unique_ptr<IScene>> scenes_{};
  std::vector<std::function<void()>> pendingOperations_{};
};

}  // namespace ls
