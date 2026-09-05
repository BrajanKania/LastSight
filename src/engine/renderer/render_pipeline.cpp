#include "engine/renderer/render_pipeline.hpp"

#include "engine/core/engine_context.hpp"
#include "engine/renderer/render_context.hpp"
#include "engine/renderer/render_phase.hpp"

namespace ls::renderer {

  void RenderPipeline::onResize(int width, int height) {
    width_ = width;
    height_ = height;

    for (auto& [phase, passes] : passes_) {
      for (auto& pass : passes) {
        pass->onResize(width_, height_);
      }
    }
  }

  void RenderPipeline::execute(const EngineContext& engineCtx) {
    frameData_.clear();

    static constexpr RenderPhase kExecutionOrder[]{
      RenderPhase::PrePass, RenderPhase::MainPass,  RenderPhase::PostProcessPass,
      RenderPhase::UIPass,  RenderPhase::DebugPass,
    };

    RenderContext renderCtx{
      .cmdBuffer = cmdBuffer_,
      .frameData = frameData_,
      .materialManager = *engineCtx.materialManager,
      .textureManager = *engineCtx.textureManager,
    };

    for (const auto& phase : kExecutionOrder) {
      if (!passes_.contains(phase))
        continue;

      for (auto& pass : passes_[phase]) {
        pass->execute(renderCtx);
      }
    }

    cmdBuffer_.clear();
  }

  std::size_t RenderPipeline::getPassCount() const {
    std::size_t count{ 0 };
    for (const auto& phasePasses : passes_) {
      count += phasePasses.second.size();
    }
    return count;
  }

  IRenderPass* RenderPipeline::getPass(std::size_t index) {
    std::size_t i{ 0 };
    for (const auto& pahasePasses : passes_) {
      for (const auto& pass : pahasePasses.second) {
        if (i == index)
          return pass.get();
        ++i;
      }
    }

    return nullptr;
  }
}  // namespace ls::renderer
