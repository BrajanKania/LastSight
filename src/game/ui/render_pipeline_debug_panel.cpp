#include "game/ui/render_pipeline_debug_panel.hpp"

#include <imgui.h>

#include <cstdint>
#include <format>

#include "engine/renderer/framebuffer.hpp"

namespace ls::ui {

  namespace {
    void drawTexturePreview(const Framebuffer* fbo, float scale) {
      ImTextureID textureId{ (ImTextureID)(uintptr_t)fbo->getColorBufferID() };
      ImGui::Image(textureId, ImVec2(fbo->getWidth() * scale, fbo->getHeight() * scale), ImVec2(0, 1), ImVec2(1, 0));
    }

  }  // namespace

  RenderPipelineDebugPanel::RenderPipelineDebugPanel(RenderPipeline& renderPipeline)
      : renderPipeline_{ renderPipeline } {}

  void RenderPipelineDebugPanel::render(const UIContext& ctx) {
    ImGui::Begin("Render Pipeline Debugger");

    {  // render pass list
      ImGui::SeparatorText("Render Passes");

      for (std::size_t i{ 0 }; i < renderPipeline_.getPassCount(); i++) {
        auto* pass{ renderPipeline_.getPass(i) };

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
      if (selectedPassIndex_ >= 0 && selectedPassIndex_ < static_cast<int>(renderPipeline_.getPassCount())) {
        auto* pass{ renderPipeline_.getPass(selectedPassIndex_) };
        Framebuffer* targetFBO{ pass->getTargetFBO() };

        if (targetFBO) {
          ImGui::Text(
              "Resolution: [%i, %i], Id: %i",
              targetFBO->getWidth(),
              targetFBO->getHeight(),
              targetFBO->getColorBufferID()
          );
          drawTexturePreview(targetFBO, imageScale_);
        } else {
          ImGui::Text("No target FBO.");
        }
      } else {
        ImGui::Text("Select a render pass from the list above.");
      }
    }

    ImGui::End();
  }

}  // namespace ls::ui
