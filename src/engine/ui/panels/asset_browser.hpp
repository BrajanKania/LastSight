#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class AssetBrowserPanel : public IUIPanel {
  public:
    AssetBrowserPanel() = default;

    virtual void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
