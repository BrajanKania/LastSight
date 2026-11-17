#pragma once

#include <filesystem>

namespace ls::event {

  struct RequestRemovePath {
    std::filesystem::path path{};
  };

}  // namespace ls::event
