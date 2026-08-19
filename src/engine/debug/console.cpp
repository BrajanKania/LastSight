#include "engine/debug/console.hpp"

#include <format>
#include <sstream>
#include <string>

#include "engine/debug/command.hpp"
#include "engine/debug/log_entry.hpp"
#include "engine/debug/log_level.hpp"

namespace ls::debug {

  void Console::log(const std::string& message, LogLevel level) {
    logs_.push_back(
        LogEntry{
            .message = message,
            .level = level,
        }
    );
  }

  void Console::execute(const std::string& strCommand) {
    if (strCommand.empty())
      return;

    log(std::format("> {}", strCommand), LogLevel::Info);

    std::stringstream ss{ strCommand };
    std::string name;
    ss >> name;

    CommandArgs args;
    std::string arg;
    while (ss >> arg) {
      args.push_back(arg);
    }

    const auto& it{ commands_.find(name) };
    if (it != commands_.end()) {
      it->second.command(args);
    } else {
      log(std::format("Unknown command: {}", strCommand), LogLevel::Error);
    }
  }

  void Console::clear() { logs_.clear(); }

  void Console::registerCommand(const std::string& name, const std::string& description, CommandCallback command) {
    commands_.emplace(
        name,
        CommandInfo{
            .description = description,
            .command = std::move(command),
        }
    );
  }

}  // namespace ls::debug
