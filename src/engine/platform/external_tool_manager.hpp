#pragma once

#include <filesystem>

namespace ls::platform {

  class ExternalToolManager {
  public:
    void openInEditor(const std::filesystem::path& path);
  };

}  // namespace ls::platform
