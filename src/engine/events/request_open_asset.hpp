#pragma once

#include <filesystem>

namespace ls::event {

  struct RequestOpenAsset {
    std::filesystem::path path;
  };

}  // namespace ls::event
