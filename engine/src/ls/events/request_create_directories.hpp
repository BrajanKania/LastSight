#pragma once

#include <filesystem>

namespace ls::event {

  struct RequestCreateDirectories {
    std::filesystem::path path{};
  };

}  // namespace ls::event
