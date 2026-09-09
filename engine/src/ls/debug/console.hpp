#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ls/debug/command.hpp"
#include "ls/debug/log_entry.hpp"
#include "ls/debug/log_level.hpp"

namespace ls::debug {

  class Console {
  public:
    void log(const std::string& message, LogLevel level);
    void execute(const std::string& strCommand);
    void clear();

    const std::vector<LogEntry>& getLogs() const { return logs_; };

    void registerCommand(const std::string& name, const std::string& description, CommandCallback command);
    const std::unordered_map<std::string, CommandInfo>& getCommands() const { return commands_; }

  private:
    std::vector<LogEntry> logs_;
    std::unordered_map<std::string, CommandInfo> commands_;
  };

}  // namespace ls::debug
