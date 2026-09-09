#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ls/core/engine_context.hpp"
#include "ls/core/i_scene.hpp"
#include "ls/core/scene_context.hpp"
#include "ls/input/input_context.hpp"

namespace ls {

  class SceneManager {
  public:
    using SceneFactory = std::function<std::unique_ptr<IScene>()>;

    explicit SceneManager(EngineContext engineCtx, int width, int height)
        : engineCtx_{ engineCtx },
          width_{ width },
          height_{ height } {}

    template <typename TScene>
    void registerScene(const std::string& name) {
      factories_[name] = [this]() -> std::unique_ptr<IScene> { return std::make_unique<TScene>(engineCtx_); };
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

    void handleInput(input::InputContext& inputCtx);
    void update(float dt);
    void render();
    void renderUI();
    void onResize(int width, int height);

    std::vector<std::string> getRegisteredSceneNames() const;

    SceneContext getActiveSceneContext() {
      if (scenes_.empty())
        return SceneContext{};

      return scenes_.back()->getSceneContext();
    }

    const char* getActiveSceneName() const {
      if (scenes_.empty())
        return "";

      return scenes_.back()->getName();
    }

  private:
    void processPendingOperations();

    EngineContext engineCtx_;
    int width_{ 0 };
    int height_{ 0 };

    std::unordered_map<std::string, SceneFactory> factories_{};
    std::vector<std::unique_ptr<IScene>> scenes_{};
    std::vector<std::function<void()>> pendingOperations_{};
  };

}  // namespace ls
