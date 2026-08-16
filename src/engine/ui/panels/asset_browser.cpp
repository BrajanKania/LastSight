#include "engine/ui/panels/asset_browser.hpp"

#include <imgui.h>

#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void AssetBrowserPanel::render(const UIContext& ctx) {
    if (ImGui::Begin(ui::panel::kAssetBrowser)) {
      ImGui::Text("Assets");
    }
    ImGui::End();
  }

}  // namespace ls::ui
