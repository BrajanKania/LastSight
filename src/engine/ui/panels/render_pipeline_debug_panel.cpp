#include "engine/ui/panels/render_pipeline_debug_panel.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdint>
#include <format>

#include "engine/gfx/framebuffer.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  namespace {

    void drawInteractiveCanvas(const gfx::Framebuffer* fbo, ImVec2& pan, float& zoom) {
      const ImVec2 canvasSize{ ImGui::GetContentRegionAvail() };

      const ImVec2 canvasPosition{ ImGui::GetCursorScreenPos() };
      ImDrawList* drawList{ ImGui::GetWindowDrawList() };

      drawList->AddRectFilled(
          canvasPosition,
          ImVec2(canvasPosition.x + canvasSize.x, canvasPosition.y + canvasSize.y),
          IM_COL32(0, 0, 0, 255)
      );

      ImGui::InvisibleButton(
          "##CanvasInput", canvasSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle
      );
      const bool isHovered{ ImGui::IsItemHovered() };
      const bool isActive{ ImGui::IsItemActive() };

      ImGuiIO& io{ ImGui::GetIO() };

      if (isHovered && io.MouseWheel != 0.0f) {
        zoom = std::clamp(zoom + io.MouseWheel * 0.05f, 0.1f, 2.f);
      }

      if (isActive && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) ||
                       ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))) {
        pan.x += io.MouseDelta.x;
        pan.y += io.MouseDelta.y;
      }

      const float scaledWidth{ fbo->getWidth() * zoom };
      const float scaledHeight{ fbo->getHeight() * zoom };

      const ImVec2 imgMin{ canvasPosition.x + pan.x, canvasPosition.y + pan.y };
      const ImVec2 imgMax{ imgMin.x + scaledWidth, imgMin.y + scaledHeight };

      drawList->PushClipRect(
          canvasPosition, ImVec2(canvasPosition.x + canvasSize.x, canvasPosition.y + canvasSize.y), true
      );

      ImTextureID textureId{ (ImTextureID)(uintptr_t)fbo->getColorBufferId() };
      drawList->AddImage(textureId, imgMin, imgMax, ImVec2(0, 1), ImVec2(1, 0));

      drawList->AddRect(imgMin, imgMax, IM_COL32(255, 255, 255, 60));

      drawList->PopClipRect();
    }

  }  // namespace

  void RenderPipelineDebugPanel::render(const UIContext& ctx) {
    if (!ctx.sceneCtx.renderPipeline) {
      selectedPassIndex_ = -1;
      return;
    }

    if (ImGui::Begin(ui::panel::kRenderPipelineDebug, &visible_)) {
      ImGuiTableFlags tableFlags{ ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                                  ImGuiTableFlags_SizingStretchProp };

      if (ImGui::BeginTable("##RenderPipelineDebugLayout", 2, tableFlags)) {
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch, 0.40f);
        ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthStretch, 0.60f);

        ImGui::TableNextColumn();

        if (ImGui::BeginTable("##RenderPassesLayout", 2, tableFlags)) {
          ImGui::TableSetupColumn("PassList", ImGuiTableColumnFlags_WidthStretch, 0.40f);
          ImGui::TableSetupColumn("PassInfo", ImGuiTableColumnFlags_WidthStretch, 0.60f);

          ImGui::TableNextColumn();
          ImGui::SeparatorText("Render Passes");

          for (std::size_t i{ 0 }; i < ctx.sceneCtx.renderPipeline->getPassCount(); i++) {
            auto* pass{ ctx.sceneCtx.renderPipeline->getPass(i) };
            if (!pass)
              continue;

            ImGui::PushID(static_cast<int>(i));

            bool isSelected{ static_cast<int>(i) == selectedPassIndex_ };
            std::string label{ std::format("[{}] {}", i, pass->getName()) };

            if (ImGui::Selectable(label.c_str(), isSelected)) {
              selectedPassIndex_ = isSelected ? -1 : static_cast<int>(i);
            }

            ImGui::PopID();
          }

          bool isSelectedPass{ selectedPassIndex_ >= 0 &&
                               selectedPassIndex_ < static_cast<int>(ctx.sceneCtx.renderPipeline->getPassCount()) };

          ImGui::TableNextColumn();
          ImGui::SeparatorText("Pass Info");

          if (isSelectedPass) {
            auto* pass{ ctx.sceneCtx.renderPipeline->getPass(selectedPassIndex_) };
            if (pass) {
              ImGui::Text("Name: %s", pass->getName());
            }

            gfx::Framebuffer* targetFBO{ pass ? pass->getTargetFBO() : nullptr };
            if (targetFBO) {
              ImGui::Text("Resolution: %i x %i", targetFBO->getWidth(), targetFBO->getHeight());
              ImGui::Text("ID: %u", targetFBO->getColorBufferId());
            } else {
              ImGui::TextDisabled("Selected pass has no target FBO.");
            }
          } else {
            ImGui::TextDisabled("Select pass from the list.");
          }

          ImGui::EndTable();
        }

        ImGui::SeparatorText("Canvas Controls");
        if (ImGui::Button("Reset View", ImVec2(-1.0f, 0.0f))) {
          canvasPan_ = ImVec2(10.0f, 10.0f);
          canvasZoom_ = 0.3f;
        }
        ImGui::SetNextItemWidth(-1.f);
        ImGui::DragFloat("##zoom", &canvasZoom_, 0.01f, 0.1f, 2.f, "Zoom %.2fx");

        ImGui::TableNextColumn();
        bool isSelectedPass{ selectedPassIndex_ >= 0 &&
                             selectedPassIndex_ < static_cast<int>(ctx.sceneCtx.renderPipeline->getPassCount()) };
        if (isSelectedPass) {
          auto* pass{ ctx.sceneCtx.renderPipeline->getPass(selectedPassIndex_) };
          gfx::Framebuffer* targetFBO{ pass ? pass->getTargetFBO() : nullptr };

          if (targetFBO) {
            drawInteractiveCanvas(targetFBO, canvasPan_, canvasZoom_);
          } else {
            ImGui::TextDisabled("Selected pass has no target FBO.");
          }
        } else {
          ImGui::TextDisabled("Selected pass from the list.");
        }

        ImGui::EndTable();
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
