#include "engine/core/scene_manager.hpp"

#include <imgui.h>

#include "engine/ui/ui_system.hpp"

namespace ls {

  void SceneManager::popScene() {
    pendingOperations_.push_back([this]() {
      if (!scenes_.empty()) {
        scenes_.back()->onExit();
        scenes_.pop_back();
      }
    });
  }

  void SceneManager::handleInput() {
    if (!scenes_.empty()) {
      scenes_.back()->handleInput();
    }
  }

  void SceneManager::update(float dt) {
    processPendingOperations();

    if (!scenes_.empty()) {
      scenes_.back()->update(dt);
    }
  }

  void SceneManager::render() {
    ui_system::beginFrame();

    if (!scenes_.empty()) {
      auto firstToRender{ scenes_.size() - 1 };
      while (firstToRender > 0 && !scenes_[firstToRender]->isOpaque())
        firstToRender--;

      for (size_t i{ firstToRender }; i < scenes_.size(); i++) {
        scenes_[i]->render();
      }
    }

    ui_system::endFrame();
  }

  void SceneManager::processPendingOperations() {
    for (auto& op : pendingOperations_) {
      op();
    }
    pendingOperations_.clear();
  }

  void SceneManager::onResize(int width, int height) {
    for (auto& scene : scenes_)
      scene->onResize(width, height);
  }

}  // namespace ls
