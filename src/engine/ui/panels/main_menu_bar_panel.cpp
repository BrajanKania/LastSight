#include "engine/ui/panels/main_menu_bar_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/core/engine_mode.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_console_auto_scroll.hpp"
#include "engine/events/request_change_engine_mode.hpp"
#include "engine/events/request_change_ui_style.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "engine/events/request_reload_textures.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/ui/panels/panel_names.hpp"
#include "engine/ui/ui_style.hpp"

namespace ls::ui {

  void MainMenuBarPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "MainMenuBarPanel requires a valid EventQueue!");

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Engine")) {
        if (ImGui::BeginMenu("Reload")) {
          if (ImGui::MenuItem("Textures")) {
            ctx.engineCtx.eventQueue->publish(event::RequestReloadTextures{});
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Preferences") && ctx.editorPreferences != nullptr) {
          if (ImGui::BeginMenu("Style")) {
            const bool isDark{ ctx.editorPreferences->uiStyle == UIStyle::Dark };
            const bool isClassic{ ctx.editorPreferences->uiStyle == UIStyle::Classic };
            const bool isLight{ ctx.editorPreferences->uiStyle == UIStyle::Light };

            if (ImGui::MenuItem("Dark", nullptr, isDark)) {
              ctx.engineCtx.eventQueue->publish(
                  event::RequestChangeUIStyle{
                      .newStyle = UIStyle::Dark,
                  }
              );
            }

            if (ImGui::MenuItem("Classic", nullptr, isClassic)) {
              ctx.engineCtx.eventQueue->publish(
                  event::RequestChangeUIStyle{
                      .newStyle = UIStyle::Classic,
                  }
              );
            }

            if (ImGui::MenuItem("Light", nullptr, isLight)) {
              ctx.engineCtx.eventQueue->publish(
                  event::RequestChangeUIStyle{
                      .newStyle = UIStyle::Light,
                  }
              );
            }

            ImGui::EndMenu();
          }

          if (ImGui::BeginMenu("Console")) {
            bool enableAutoScroll{ ctx.editorPreferences->consoleAutoScroll };
            if (ImGui::MenuItem("Auto Scroll", nullptr, &enableAutoScroll)) {
              ctx.engineCtx.eventQueue->publish(
                  event::RequestChangeConsoleAutoScroll{
                      .enable = enableAutoScroll,
                  }
              );
            }
            ImGui::EndMenu();
          }

          ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Close", "Ecs")) {
          ctx.engineCtx.eventQueue->publish(event::RequestQuitEngine{});
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Save", "Ctrl + S")) {
          ctx.engineCtx.eventQueue->publish(event::RequestSaveScene{});
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Panels")) {
        const bool hasSceneManager{ ctx.engineCtx.sceneManager != nullptr };
        const bool hasRegistry{ ctx.sceneCtx.registry != nullptr };

        if (ImGui::MenuItem(ui::panel::kSceneBrowser, nullptr, false, hasSceneManager)) {
          ctx.engineCtx.eventQueue->publish(
              event::SetPanelVisibility{
                  .name = ui::panel::kSceneBrowser,
                  .visible = true,
              }
          );
        }

        if (ImGui::MenuItem(ui::panel::kAssetBrowser, nullptr, false, hasSceneManager)) {
          ctx.engineCtx.eventQueue->publish(
              event::SetPanelVisibility{
                  .name = ui::panel::kAssetBrowser,
                  .visible = true,
              }
          );
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ui::panel::kSceneHierarchy, nullptr, false, hasRegistry)) {
          ctx.engineCtx.eventQueue->publish(
              event::SetPanelVisibility{
                  .name = ui::panel::kSceneHierarchy,
                  .visible = true,
              }
          );
        }

        const bool hasInspectorDeps{ hasRegistry && ctx.engineCtx.textureManager != nullptr };
        if (ImGui::MenuItem(ui::panel::kEntityInspector, nullptr, false, hasInspectorDeps)) {
          ctx.engineCtx.eventQueue->publish(
              event::SetPanelVisibility{
                  .name = ui::panel::kEntityInspector,
                  .visible = true,
              }
          );
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Debug")) {
        const bool hasRenderPipeline{ ctx.engineCtx.renderPipeline != nullptr };
        if (ImGui::MenuItem(ui::panel::kRenderPipelineDebug, nullptr, false, hasRenderPipeline)) {
          ctx.engineCtx.eventQueue->publish(
              event::SetPanelVisibility{
                  .name = ui::panel::kRenderPipelineDebug,
                  .visible = true,
              }
          );
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Mode") && ctx.engineCtx.engineMode != nullptr) {
        const bool isEditMode{ *ctx.engineCtx.engineMode == EngineMode::Edit };
        const bool isPlayMode{ *ctx.engineCtx.engineMode == EngineMode::Play };

        if (ImGui::MenuItem("Play", nullptr, isPlayMode)) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestChangeEngineMode{
                  .newMode = EngineMode::Play,
              }
          );
        }

        if (ImGui::MenuItem("Edit", nullptr, isEditMode)) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestChangeEngineMode{
                  .newMode = EngineMode::Edit,
              }
          );
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }

}  // namespace ls::ui
