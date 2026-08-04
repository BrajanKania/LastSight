#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "engine/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class RenderPipeline {
  public:
    template <typename T, typename... Args>
    void addPass(Args&&... args) {
      auto newPass{ std::make_unique<T>(std::forward<Args>(args)...) };
      newPass->onEnter();
      passes_.push_back(std::move(newPass));
    }

    void execute(const RenderContext& ctx) {
      for (auto& pass : passes_) {
        pass->execute(ctx);
      }
    }

    std::size_t getPassCount() const { return passes_.size(); }

    IRenderPass* getPass(std::size_t index) {
      if (index < passes_.size())
        return passes_[index].get();
      return nullptr;
    }

  private:
    std::vector<std::unique_ptr<IRenderPass>> passes_{};
  };

}  // namespace ls::renderer
