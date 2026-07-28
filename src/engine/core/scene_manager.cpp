#include "engine/core/scene_manager.hpp"

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
    if (!scenes_.empty()) {
      scenes_.back()->update(dt);
    }
    processPendingOperations();
  }

  void SceneManager::render() {
    if (scenes_.empty())
      return;

    auto firstToRender{scenes_.size() - 1};
    while (firstToRender > 0 && !scenes_[firstToRender]->isOpaque())
      firstToRender--;

    for (size_t i{firstToRender}; i < scenes_.size(); i++) {
      scenes_[i]->render();
    }
  }

  void SceneManager::processPendingOperations() {
    for (auto& op : pendingOperations_) {
      op();
    }
    pendingOperations_.clear();
  }

}  // namespace ls
