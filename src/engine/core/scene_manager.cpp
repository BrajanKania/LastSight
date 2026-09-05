#include "engine/core/scene_manager.hpp"

#include <imgui.h>

#include "engine/input/input_context.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/renderer/render_system.hpp"

namespace ls {

  void SceneManager::changeScene(const std::string& name) {
    pendingOperations_.push_back([this, name]() {
      auto it{ factories_.find(name) };
      if (it == factories_.end())
        return;

      while (!scenes_.empty()) {
        scenes_.back()->onExit();
        scenes_.pop_back();
      }

      auto newScene{ it->second() };
      newScene->onEnter();

      if (width_ > 0 && height_ > 0) {
        newScene->onResize(width_, height_);
        engineCtx_.renderPipeline->onResize(width_, height_);
      }

      scenes_.push_back(std::move(newScene));
    });
  }

  void SceneManager::popScene() {
    pendingOperations_.push_back([this]() {
      if (!scenes_.empty()) {
        scenes_.back()->onExit();
        scenes_.pop_back();
      }
    });
  }

  void SceneManager::handleInput(input::InputContext& inputCtx) {
    if (!scenes_.empty()) {
      scenes_.back()->handleInput(inputCtx);
    }
  }

  void SceneManager::update(float dt) {
    processPendingOperations();

    if (!scenes_.empty()) {
      scenes_.back()->update(dt);
    }
  }

  void SceneManager::render() {
    if (scenes_.empty())
      return;

    render_system::setViewport(0, 0, width_, height_);

    auto firstToRender{ scenes_.size() - 1 };
    while (firstToRender > 0 && !scenes_[firstToRender]->isOpaque())
      firstToRender--;

    for (size_t i{ firstToRender }; i < scenes_.size(); i++) {
      scenes_[i]->render();
    }
  }

  void SceneManager::renderUI() {
    if (scenes_.empty())
      return;

    auto firstToRender{ scenes_.size() - 1 };
    while (firstToRender > 0 && !scenes_[firstToRender]->isOpaque())
      firstToRender--;

    for (size_t i{ firstToRender }; i < scenes_.size(); i++) {
      scenes_[i]->renderUI();
    }
  }

  void SceneManager::onResize(int width, int height) {
    pendingOperations_.push_back([this, width, height]() {
      width_ = width;
      height_ = height;
      for (auto& scene : scenes_) {
        scene->onResize(width_, height_);
      }
    });
  }

  std::vector<std::string> SceneManager::getRegisteredSceneNames() const {
    std::vector<std::string> names;
    names.reserve(factories_.size());
    for (const auto& [name, _] : factories_) {
      names.push_back(name);
    }
    return names;
  }

  void SceneManager::processPendingOperations() {
    for (auto& op : pendingOperations_) {
      op();
    }
    pendingOperations_.clear();
  }

}  // namespace ls
