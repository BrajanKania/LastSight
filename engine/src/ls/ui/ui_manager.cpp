#include "ls/ui/ui_manager.hpp"

namespace ls::ui {

  void UIManager::render(const UIContext& ctx) {
    for (auto& panel : panels_) {
      if (panel->isVisible()) {
        panel->render(ctx);
      }
    }
  }

  void UIManager::clear() {
    panels_.clear();
    nameToIndex_.clear();
  }

}  // namespace ls::ui
