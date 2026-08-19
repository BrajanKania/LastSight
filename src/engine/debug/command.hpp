#pragma once

#include <functional>
#include <string>
#include <vector>

namespace ls::debug {

  using CommandArgs = std::vector<std::string>;
  using CommandCallback = std::function<void(const CommandArgs&)>;

  struct CommandInfo {
    std::string description;
    CommandCallback command;
  };

}  // namespace ls::debug
