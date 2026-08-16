#include "engine/core/editor_layer.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <cassert>

#include "engine/actions/toggle_engine_mode.hpp"
#include "engine/core/engine_context.hpp"
#include "engine/core/engine_mode.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/engine_mode_changed.hpp"
#include "engine/events/request_change_engine_mode.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/input/types.hpp"
#include "engine/ui/panels/asset_browser.hpp"
#include "engine/ui/panels/entity_inspector_panel.hpp"
#include "engine/ui/panels/main_menu_bar_panel.hpp"
#include "engine/ui/panels/panel_names.hpp"
#include "engine/ui/panels/render_pipeline_debug_panel.hpp"
#include "engine/ui/panels/scene_browser_panel.hpp"
#include "engine/ui/panels/scene_hierarchy_panel.hpp"
#include "engine/ui/panels/viewport_panel.hpp"

namespace ls {

  EditorLayer::EditorLayer() {
    inputManager_.bindKey<action::ToggleEngineMode>(input::Key::Grave);

    uiManager_.addPanel<ui::MainMenuBarPanel>(ui::panel::kMainMenuBar);
    uiManager_.addPanel<ui::SceneBrowserPanel>(ui::panel::kSceneBrowser);
    uiManager_.addPanel<ui::AssetBrowserPanel>(ui::panel::kAssetBrowser);
    uiManager_.addPanel<ui::RenderPipelineDebugPanel>(ui::panel::kRenderPipelineDebug);
    uiManager_.addPanel<ui::SceneHierarchyPanel>(ui::panel::kSceneHierarchy);
    uiManager_.addPanel<ui::EntityInspectorPanel>(ui::panel::kEntityInspector);
    uiManager_.addPanel<ui::ViewportPanel>(ui::panel::kViewport);
    uiManager_.getPanel<ui::ViewportPanel>(ui::panel::kViewport).setVisible(true);
  }

  void EditorLayer::handleInput(EngineContext engineCtx, bool blockKeyboard, bool blockMouse) {
    assert(engineCtx.eventQueue != nullptr && "[EditorLayer] Requires a valid EventQueue!");
    assert(engineCtx.engineMode != nullptr && "[EditorLayer] Requires a valid EngineMode!");

    inputManager_.update(blockKeyboard, blockMouse);

    auto toggleDebugActionState{ inputManager_.getActionState<action::ToggleEngineMode>() };
    if (toggleDebugActionState == input::ActionState::JustPressed) {
      engineCtx.eventQueue->publish(
          event::RequestChangeEngineMode{
              .newMode = (*engineCtx.engineMode == EngineMode::Edit ? EngineMode::Play : EngineMode::Edit),
          }
      );
    }
  }

  void EditorLayer::update(EngineContext engineCtx) {
    assert(engineCtx.eventQueue != nullptr && "[EditorLayer] Requires a valid EventQueue!");
    handleRequest(*engineCtx.eventQueue);
  }

  void EditorLayer::render(EngineContext engineCtx, const SceneContext& sceneCtx) {
    assert(engineCtx.engineMode != nullptr && "[EditorLayer] Requires a valid EngineMode!");
    bool isEditMode{ *engineCtx.engineMode == EngineMode::Edit };

    if (isEditMode) {
      renderDockSpace();
    }

    uiManager_.render(
        ui::UIContext{
            .engineCtx = engineCtx,
            .sceneCtx = sceneCtx,
            .selectionCtx = &selectionCtx_,
        }
    );
  }

  bool EditorLayer::isViewportHovered() const {
    return uiManager_.getPanel<ui::ViewportPanel>(ui::panel::kViewport).isHovered();
  }

  bool EditorLayer::isViewportFocused() const {
    return uiManager_.getPanel<ui::ViewportPanel>(ui::panel::kViewport).isFocused();
  }

  void EditorLayer::handleRequest(dispatch::EventQueue& engineEventQueue) {
    for (const auto& event : engineEventQueue.getEvents<event::EngineModeChanged>()) {
      bool isEditMode{ event.newMode == EngineMode::Edit };
      uiManager_.getPanel(ui::panel::kMainMenuBar).setVisible(isEditMode);
      uiManager_.getPanel(ui::panel::kSceneBrowser).setVisible(isEditMode);
      uiManager_.getPanel(ui::panel::kAssetBrowser).setVisible(isEditMode);
      uiManager_.getPanel(ui::panel::kSceneHierarchy).setVisible(isEditMode);
      uiManager_.getPanel(ui::panel::kEntityInspector).setVisible(isEditMode);
      uiManager_.getPanel(ui::panel::kRenderPipelineDebug).setVisible(isEditMode);

      shouldResetLayout_ = isEditMode;
    }

    for (const auto& event : engineEventQueue.getEvents<event::TogglePanel>()) {
      uiManager_.getPanel(event.name).toggleVisible();
    }

    for (const auto& event : engineEventQueue.getEvents<event::SetPanelVisibility>()) {
      uiManager_.getPanel(event.name).setVisible(event.visible);
    }
  }

  void EditorLayer::renderDockSpace() {
    ImGuiID dockspaceId{ ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport()) };

    if (shouldResetLayout_) {
      shouldResetLayout_ = false;

      ImGui::DockBuilderRemoveNode(dockspaceId);
      ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

      ImGuiID dockMain{ dockspaceId };
      ImGuiID dockRight{ ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain) };
      ImGuiID dockBottom{ ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain) };

      ImGuiID dockRightTop;
      ImGuiID dockRightBottom{ ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.6f, nullptr, &dockRightTop) };

      ImGui::DockBuilderDockWindow(ui::panel::kViewport, dockMain);

      ImGui::DockBuilderDockWindow(ui::panel::kSceneBrowser, dockBottom);
      ImGui::DockBuilderDockWindow(ui::panel::kAssetBrowser, dockBottom);
      ImGui::DockBuilderDockWindow(ui::panel::kRenderPipelineDebug, dockBottom);

      ImGui::DockBuilderDockWindow(ui::panel::kSceneHierarchy, dockRightTop);
      ImGui::DockBuilderDockWindow(ui::panel::kEntityInspector, dockRightBottom);

      ImGui::DockBuilderFinish(dockspaceId);
    }
  }

}  // namespace ls
