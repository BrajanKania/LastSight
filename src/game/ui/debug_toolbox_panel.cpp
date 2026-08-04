#include "game/ui/debug_toolbox_panel.hpp"

#include <imgui.h>

#include "engine/events/toggle_panel.hpp"
#include "game/ui/panel_names.hpp"

namespace ls::ui {

  void DebugToolboxPanel::render(const UIContext& ctx) {
    if (ImGui::Begin("Debug Toolbox", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImVec2 buttonSize{ 200.f, 0.f };

      if (ImGui::Button("Entity Explorer", buttonSize)) {
        ctx.eventQueue.publish<event::TogglePanel>(event::TogglePanel{ .name = ui::panel::kEntityExplorer });
      }

      if (ImGui::Button("Render Pipeline Debugger", buttonSize)) {
        ctx.eventQueue.publish(event::TogglePanel(event::TogglePanel{ .name = ui::panel::kRenderPipelineDebug }));
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
