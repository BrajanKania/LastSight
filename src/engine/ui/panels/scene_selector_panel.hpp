#pragma once

#include "engine/core/scene_manager.hpp"
#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class SceneSelectorPanel : public IUIPanel {
  public:
    explicit SceneSelectorPanel(const SceneManager& sceneManager);

    virtual void render(const UIContext& ctx) override;

  private:
    const SceneManager& sceneManager_;
  };

}  // namespace ls::ui
