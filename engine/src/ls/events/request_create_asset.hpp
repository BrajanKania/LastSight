#pragma once

#include <filesystem>
#include <string>

#include "ls/asset/asset_types.hpp"

namespace ls::event {

  struct RequestCreateAsset {
    std::string name{};
    asset::AssetType type{};
    std::filesystem::path path{};
  };

}  // namespace ls::event
