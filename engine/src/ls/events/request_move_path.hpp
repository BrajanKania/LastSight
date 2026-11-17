#pragma once

#include <filesystem>

namespace ls::event {

  struct RequestMovePath {
    std::filesystem::path oldPath{};
    std::filesystem::path newPath{};
  };

}  // namespace ls::event
