#include "engine/platform/external_tool_manager.hpp"

#include <filesystem>
#include <thread>

namespace ls::platform {

  void ExternalToolManager::openInEditor(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path))
      return;

    std::thread([absPath = std::filesystem::absolute(path)]() {

#if defined(__linux__)
      const std::string nvimCmd{ std::format("x-terminal-emulator -e nvim \"{}\"", absPath.string()) };
      std::system(nvimCmd.c_str());
#endif
    }).detach();
  }

}  // namespace ls::platform
