#include "engine/ui/panels/render_pipeline_debug_panel.hpp"

#include <imgui.h>

#include <cstdint>
#include <format>

#include "engine/gfx/framebuffer.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  namespace {
    void drawTexturePreview(const gfx::Framebuffer* fbo, float scale) {
      ImTextureID textureId{ (ImTextureID)(uintptr_t)fbo->getColorBufferId() };
      ImGui::Image(textureId, ImVec2(fbo->getWidth() * scale, fbo->getHeight() * scale), ImVec2(0, 1), ImVec2(1, 0));
    }

  }  // namespace

  void RenderPipelineDebugPanel::render(const UIContext& ctx) {
    if (!ctx.sceneCtx.renderPipeline) {
      selectedPassIndex_ = -1;
      imageScale_ = 0.2f;
      return;
    }

    if (ImGui::Begin(ui::panel::kRenderPipelineDebug, &visible_)) {
      {  // render pass list
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
      }

      {  // Framebuffer preview
        ImGui::SeparatorText("Preview Properties");
        ImGui::SliderFloat("Scale", &imageScale_, 0.1f, 1.f);

        ImGui::Separator();
        if (selectedPassIndex_ >= 0 &&
            selectedPassIndex_ < static_cast<int>(ctx.sceneCtx.renderPipeline->getPassCount())) {
          auto* pass{ ctx.sceneCtx.renderPipeline->getPass(selectedPassIndex_) };
          gfx::Framebuffer* targetFBO{ pass->getTargetFBO() };

          if (targetFBO) {
            ImGui::Text(
                "Resolution: [%i, %i], Id: %i",
                targetFBO->getWidth(),
                targetFBO->getHeight(),
                targetFBO->getColorBufferId()
            );
            drawTexturePreview(targetFBO, imageScale_);
          } else {
            ImGui::Text("No target FBO.");
          }
        } else {
          ImGui::Text("Select a render pass from the list above.");
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
