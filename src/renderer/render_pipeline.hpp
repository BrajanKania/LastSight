#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "render_pass.hpp"

namespace ls {

  class RenderPipeline {
  public:
    template <typename T>
    void addPass() {
      auto newPass{std::make_unique<T>()};
      newPass->onEnter();
      passes_.push_back(std::move(newPass));
    }

    void execute(const RenderContext& ctx) {
      for (auto& pass : passes_) {
        pass->execute(ctx);
      }
    }

  private:
    std::vector<std::unique_ptr<IRenderPass>> passes_{};
  };

}  // namespace ls
