#include "engine/ui/panels/viewport_panel.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdint>

#include "engine/dispatch/event_queue.hpp"
#include "engine/events/viewport_resized.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/input/input_system.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void ViewportPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.engineMode != nullptr && "[ViewportPanel] Requires a valid EngineMode!");
    assert(ctx.engineCtx.renderPipeline != nullptr && "[ViewportPanel] Requires a valid RenderPipeline!");
    assert(ctx.engineCtx.eventQueue != nullptr && "[ViewportPanel] Requires a valid EventQueue!");

    const bool isPlayMode{ *ctx.engineCtx.engineMode == EngineMode::Play };

    ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus };

    if (isPlayMode) {
      const ImGuiViewport* mainViewport{ ImGui::GetMainViewport() };
      ImGui::SetNextWindowPos(mainViewport->WorkPos);
      ImGui::SetNextWindowSize(mainViewport->WorkSize);

      windowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    } else {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
    }

    bool visible{ isVisible() };
    if (ImGui::Begin(ui::panel::kViewport, isPlayMode ? nullptr : &visible, windowFlags)) {
      isFocused_ = ImGui::IsWindowFocused();

      ImVec2 viewportSize{ ImGui::GetContentRegionAvail() };
      ImVec2 viewportPosition{ ImGui::GetCursorScreenPos() };

      ImVec2 cursorScreenPosition{ ImGui::GetCursorScreenPos() };
      ImVec2 contentAvail{ ImGui::GetContentRegionAvail() };

      input_system::setViewportBounds(
          { cursorScreenPosition.x, cursorScreenPosition.y }, { contentAvail.x, contentAvail.y }
      );

      if (viewportSize.x != size_.x || viewportSize.y != size_.y) {
        if (viewportSize.x > 0 && viewportSize.y > 0) {
          size_ = { viewportSize.x, viewportSize.y };
          ctx.engineCtx.eventQueue->publish(
              event::ViewportResized{
                  .newSize = size_,
              }
          );
        }
      }
      if (!isPlayMode && !visible) {
        setVisible(false);
      }

      gfx::Framebuffer* targetFramebuffer{ ctx.engineCtx.renderPipeline->getFrameData().getFramebuffer("final") };

      if (targetFramebuffer) {
        std::uint32_t sceneTextureId{ targetFramebuffer->getColorBufferId() };
        ImGui::Image(
            reinterpret_cast<void*>(static_cast<uintptr_t>(sceneTextureId)), viewportSize, ImVec2(0, 1), ImVec2(1, 0)
        );
      }

      ImGui::SetCursorScreenPos(cursorScreenPosition);
      ImGui::InvisibleButton(
          "##ViewportTarget",
          viewportSize,
          ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle
      );

      isHovered_ = ImGui::IsItemHovered();
    } else {
      isFocused_ = false;
      isHovered_ = false;
    }

    ImGui::End();

    ImGui::PopStyleVar(isPlayMode ? 2 : 1);
  }

}  // namespace ls::ui
