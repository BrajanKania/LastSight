#pragma once

#include <vector>

#include "ls/renderer/render_command.hpp"

namespace ls::renderer {

  class CommandBuffer {
  public:
    template <typename TCommand>
    void push(TCommand&& cmd) {
      commands_.emplace_back(cmd);
    }

    const std::vector<RenderCommand>& getCommands() const { return commands_; }

    void clear() { commands_.clear(); }

  private:
    std::vector<RenderCommand> commands_{};
  };

}  // namespace ls::renderer
