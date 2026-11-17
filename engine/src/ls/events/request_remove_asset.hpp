#pragma once

#include "ls/asset/asset_handle.hpp"

namespace ls::event {

  struct RequestRemoveAsset {
    asset::DynamicAssetHandle handle{};
  };

}  // namespace ls::event
