#include "ls/platform/external_tool_manager.hpp"

#include <filesystem>
#include <thread>

#include "ls/asset/asset_system.hpp"

namespace ls::platform {

  void ExternalToolManager::openInEditor(const std::filesystem::path& path) {
    if (path.empty())
      return;

    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };
    if (!std::filesystem::exists(absPath))
      return;

    std::thread([absPath = std::filesystem::absolute(path)]() {

#if defined(__linux__)
      const std::string nvimCmd{ std::format("x-terminal-emulator -e nvim \"{}\"", absPath.string()) };
      std::system(nvimCmd.c_str());
#endif
    }).detach();
  }

}  // namespace ls::platform
