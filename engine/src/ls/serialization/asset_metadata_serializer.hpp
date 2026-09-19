#pragma once

#include <filesystem>

#include "ls/asset/asset_types.hpp"
#include "ls/serialization/serialization_context.hpp"

namespace ls::serialization {

  class AssetMetadataSerializer {
  public:
    AssetMetadataSerializer(SerializationContext ctx)
        : ctx_{ ctx } {}

    bool loadAssetMetadata(const std::filesystem::path& path, asset::AssetMetadata& targetMetadata);
    bool saveAssetMetadata(const std::filesystem::path& path, const asset::AssetMetadata& sourceMetadata);

  private:
    SerializationContext ctx_{};
  };

}  // namespace ls::serialization
