#include "ls/ui/panels/render_pipeline_debug_panel.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdint>

#include "ls/gfx/framebuffer.hpp"
#include "ls/renderer/render_pipeline.hpp"
#include "ls/ui/panels/panel_names.hpp"

namespace ls::ui {

  namespace {

    void drawInteractiveCanvas(const gfx::Framebuffer* fbo, ImVec2& pan, float& zoom) {
      const ImVec2 canvasSize{ ImGui::GetContentRegionAvail() };
      const ImVec2 canvasPosition{ ImGui::GetCursorScreenPos() };
      ImDrawList* drawList{ ImGui::GetWindowDrawList() };

      drawList->AddRectFilled(
          canvasPosition,
          ImVec2(canvasPosition.x + canvasSize.x, canvasPosition.y + canvasSize.y),
          IM_COL32(15, 15, 15, 255)
      );

      ImGui::InvisibleButton(
          "##CanvasInput", canvasSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle
      );
      const bool isHovered{ ImGui::IsItemHovered() };
      const bool isActive{ ImGui::IsItemActive() };

      ImGuiIO& io{ ImGui::GetIO() };

      if (isHovered && io.MouseWheel != 0.0f) {
        const float oldZoom = zoom;
        zoom = std::clamp(zoom + io.MouseWheel * 0.05f, 0.1f, 5.0f);

        const ImVec2 mouseRelPos{ io.MousePos.x - (canvasPosition.x + pan.x),
                                  io.MousePos.y - (canvasPosition.y + pan.y) };

        const float zoomFactor = zoom / oldZoom;
        pan.x -= mouseRelPos.x * (zoomFactor - 1.0f);
        pan.y -= mouseRelPos.y * (zoomFactor - 1.0f);
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
    if (!ctx.engineCtx.renderPipeline) {
      selectedFramebufferName_.clear();
      return;
    }

    if (ImGui::Begin(ui::panel::kRenderPipelineDebug, &visible_)) {
      ImGuiTableFlags tableFlags{ ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                                  ImGuiTableFlags_SizingStretchProp };

      if (ImGui::BeginTable("##RenderPipelineDebugLayout", 2, tableFlags)) {
        ImGui::TableSetupColumn("Buffers List", ImGuiTableColumnFlags_WidthStretch, 0.35f);
        ImGui::TableSetupColumn("Canvas Preview", ImGuiTableColumnFlags_WidthStretch, 0.65f);

        ImGui::TableNextColumn();
        ImGui::SeparatorText("Framebuffers");

        const auto& framebuffers{ ctx.engineCtx.renderPipeline->getFrameData().getFramebuffers() };

        if (framebuffers.empty()) {
          ImGui::TextDisabled("Add render pass to RenderPipeline...");
        } else {
          if (selectedFramebufferName_.empty() || !framebuffers.contains(selectedFramebufferName_)) {
            if (framebuffers.contains("final")) {
              selectedFramebufferName_ = "final";
            } else {
              selectedFramebufferName_ = framebuffers.begin()->first;
            }
          }

          std::vector<std::string_view> sortedNames;
          sortedNames.reserve(framebuffers.size());
          for (const auto& [name, fbo] : framebuffers) {
            sortedNames.push_back(name);
          }
          std::sort(sortedNames.begin(), sortedNames.end());

          for (const auto& name : sortedNames) {
            std::string nameStr{ name };
            ImGui::PushID(nameStr.c_str());

            bool isSelected{ nameStr == selectedFramebufferName_ };
            if (ImGui::Selectable(nameStr.c_str(), isSelected)) {
              selectedFramebufferName_ = nameStr;
            }

            ImGui::PopID();
          }
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Framebuffer Info");

        gfx::Framebuffer* selectedFramebuffer{ nullptr };
        if (framebuffers.contains(selectedFramebufferName_)) {
          selectedFramebuffer = framebuffers.at(selectedFramebufferName_);
        }

        if (selectedFramebuffer) {
          ImGui::Text("Name: %s", selectedFramebufferName_.c_str());
          ImGui::Text("Resolution: %i x %i", selectedFramebuffer->getWidth(), selectedFramebuffer->getHeight());
          ImGui::Text("Texture ID: %u", selectedFramebuffer->getColorBufferId());
        } else {
          ImGui::TextDisabled("Select a valid Framebuffer from the list.");
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Canvas Controls");
        if (ImGui::Button("Reset View", ImVec2(-1.0f, 0.0f))) {
          canvasPan_ = ImVec2(10.0f, 10.0f);
          canvasZoom_ = 0.3f;
        }
        ImGui::SetNextItemWidth(-1.f);
        ImGui::DragFloat("##zoom", &canvasZoom_, 0.01f, 0.1f, 5.0f, "Zoom %.2fx");

        ImGui::TableNextColumn();

        if (selectedFramebuffer) {
          drawInteractiveCanvas(selectedFramebuffer, canvasPan_, canvasZoom_);
        } else {
          ImGui::TextDisabled("Select Framebuffer from the list.");
        }

        ImGui::EndTable();
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
