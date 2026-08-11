#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class SceneBrowserPanel : public IUIPanel {
  public:
    SceneBrowserPanel() = default;

    virtual void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
