#pragma once

#include <filesystem>

namespace ls::event {

  struct RequestCreateFile {
    std::filesystem::path path{};
  };

}  // namespace ls::event
