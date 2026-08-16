#include "engine/ui/panels/toolbox_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/dispatch/event_queue.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void ToolboxPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "ToolboxPanel requires a valid EventQueue!");

    if (ImGui::Begin("Toolbox", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImVec2 buttonSize{ 200.f, 0.f };

      if (ImGui::Button("Scene Browser", buttonSize)) {
        if (ctx.engineCtx.sceneManager) {
          ctx.engineCtx.eventQueue->publish(event::TogglePanel(event::TogglePanel{ .name = ui::panel::kSceneBrowser }));
        }
      }

      if (ImGui::Button("Entity Inspector", buttonSize)) {
        if (ctx.sceneCtx.registry && ctx.engineCtx.textureManager) {
          ctx.engineCtx.eventQueue->publish<event::TogglePanel>(event::TogglePanel{ .name =
                                                                                        ui::panel::kEntityInspector });
        }
      }

      if (ImGui::Button("Render Pipeline Debugger", buttonSize)) {
        if (ctx.sceneCtx.renderPipeline) {
          ctx.engineCtx.eventQueue->publish(
              event::TogglePanel(event::TogglePanel{ .name = ui::panel::kRenderPipelineDebug })
          );
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
