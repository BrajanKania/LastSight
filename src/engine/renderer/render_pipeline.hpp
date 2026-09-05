#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "engine/core/engine_context.hpp"
#include "engine/renderer/command_buffer.hpp"
#include "engine/renderer/frame_data.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/render_phase.hpp"

namespace ls::renderer {

  class RenderPipeline {
  public:
    template <typename TPass, typename... Args>
    void addPass(RenderPhase phase, Args&&... args) {
      auto newPass{ std::make_unique<TPass>(std::forward<Args>(args)...) };
      newPass->onEnter();
      passes_[phase].push_back(std::move(newPass));
    }

    void onResize(int width, int height);

    void execute(const EngineContext& engineCtx);

    std::size_t getPassCount() const;

    IRenderPass* getPass(std::size_t index);

    template <typename TCommand>
    void addCommand(TCommand&& cmd) {
      cmdBuffer_.push(cmd);
    }

    void clearCommandsBuffer() { cmdBuffer_.clear(); }

    void clearPasses() { passes_.clear(); }

    FrameData& getFrameData() { return frameData_; }

    CommandBuffer& getCommandBuffer() { return cmdBuffer_; }

  private:
    int width_{ 0 };
    int height_{ 0 };

    FrameData frameData_{};
    std::unordered_map<RenderPhase, std::vector<std::unique_ptr<IRenderPass>>> passes_{};
    CommandBuffer cmdBuffer_{};
  };

}  // namespace ls::renderer
