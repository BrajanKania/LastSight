#pragma once

#include <filesystem>

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class AssetBrowserPanel : public IUIPanel {
  public:
    AssetBrowserPanel() = default;

    virtual void render(const UIContext& ctx) override;

  private:
    std::filesystem::path currentPath_{ "assets" };
    std::filesystem::path assetsPath_{ "assets" };
    char searchBuffer_[256]{};
    bool showExtensions_{ false };
  };

}  // namespace ls::ui
