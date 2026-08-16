#include "engine/ui/panels/main_menu_bar_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void MainMenuBarPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "MainMenuBarPanel requires a valid EventQueue!");

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Engine")) {
        if (ImGui::MenuItem("Close")) {
          ctx.engineCtx.eventQueue->publish(event::RequestQuitEngine{});
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Panels")) {
        if (ImGui::MenuItem(ui::panel::kSceneBrowser)) {
          if (ctx.engineCtx.sceneManager) {
            ctx.engineCtx.eventQueue->publish(
                event::SetPanelVisibility{
                    .name = ui::panel::kSceneBrowser,
                    .visible = true,
                }
            );
          }
        }
        if (ImGui::MenuItem(ui::panel::kAssetBrowser)) {
          if (ctx.engineCtx.sceneManager) {
            ctx.engineCtx.eventQueue->publish(
                event::SetPanelVisibility{
                    .name = ui::panel::kAssetBrowser,
                    .visible = true,
                }
            );
          }
        }
        if (ImGui::MenuItem(ui::panel::kSceneHierarchy)) {
          if (ctx.sceneCtx.registry) {
            ctx.engineCtx.eventQueue->publish(
                event::SetPanelVisibility{
                    .name = ui::panel::kSceneHierarchy,
                    .visible = true,
                }
            );
          }
        }
        if (ImGui::MenuItem(ui::panel::kEntityInspector)) {
          if (ctx.sceneCtx.registry && ctx.engineCtx.textureManager) {
            ctx.engineCtx.eventQueue->publish(
                event::SetPanelVisibility{
                    .name = ui::panel::kEntityInspector,
                    .visible = true,
                }
            );
          }
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Debug")) {
        if (ImGui::MenuItem(ui::panel::kRenderPipelineDebug)) {
          if (ctx.sceneCtx.renderPipeline) {
            ctx.engineCtx.eventQueue->publish(
                event::SetPanelVisibility{
                    .name = ui::panel::kRenderPipelineDebug,
                    .visible = true,
                }
            );
          }
        }
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }

}  // namespace ls::ui
