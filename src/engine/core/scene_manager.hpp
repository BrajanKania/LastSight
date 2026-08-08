#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "engine/core/i_scene.hpp"

namespace ls {

  class SceneManager {
  public:
    using SceneFactory = std::function<std::unique_ptr<IScene>()>;

    explicit SceneManager(int width, int height)
        : width_{ width },
          height_{ height } {}

    template <typename TScene>
    void registerScene(const std::string& name) {
      factories_[name] = []() -> std::unique_ptr<IScene> { return std::make_unique<TScene>(); };
    }

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

    void changeScene(const std::string& name);
    void popScene();

    void handleInput();
    void update(float dt);
    void render();
    void onResize(int width, int height);

    std::vector<std::string> getRegisteredSceneNames() const;

  private:
    void processPendingOperations();

    int width_{ 0 };
    int height_{ 0 };

    std::unordered_map<std::string, SceneFactory> factories_{};
    std::vector<std::unique_ptr<IScene>> scenes_{};
    std::vector<std::function<void()>> pendingOperations_{};
  };

}  // namespace ls
