#pragma once

#include <functional>
#include <memory>

#include "engine/core/i_scene.hpp"

namespace ls {

  class SceneManager {
  public:
    explicit SceneManager(int width, int height)
        : width_{ width },
          height_{ height } {}

    template <typename T>
    void pushScene() {
      pendingOperations_.push_back([this]() {
        auto newScene{ std::make_unique<T>() };
        newScene->onEnter();

        if (width_ > 0 && height_ > 0) {
          newScene->onResize(width_, height_);
        }

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

        auto newScene{ std::make_unique<T>() };
        newScene->onEnter();

        if (width_ > 0 && height_ > 0) {
          newScene->onResize(width_, height_);
        }

        scenes_.push_back(std::move(newScene));
      });
    }

    void handleInput();
    void update(float dt);
    void render();

    void onResize(int width, int height);

  private:
    void processPendingOperations();

    int width_{ 0 };
    int height_{ 0 };

    std::vector<std::unique_ptr<IScene>> scenes_{};
    std::vector<std::function<void()>> pendingOperations_{};
  };

}  // namespace ls
