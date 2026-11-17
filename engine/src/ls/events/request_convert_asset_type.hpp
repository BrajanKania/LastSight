#pragma once

#include <filesystem>

#include "ls/asset/asset_types.hpp"

namespace ls::event {

  struct RequestConvertAssetType {
    std::filesystem::path path{};
    asset::AssetType type{ asset::AssetType::Unknown };
  };

}  // namespace ls::event
