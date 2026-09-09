#pragma once

#include <filesystem>

#include "ls/ui/i_ui_panel.hpp"

namespace ls::ui {

  class AssetBrowserPanel : public IUIPanel {
  public:
    AssetBrowserPanel() = default;

    virtual void render(const UIContext& ctx) override;

  private:
    std::filesystem::path currentPath_{ "game/assets" };
    std::filesystem::path assetsPath_{ "game/assets" };
    char searchBuffer_[256]{};
    bool showExtensions_{ false };
  };

}  // namespace ls::ui
