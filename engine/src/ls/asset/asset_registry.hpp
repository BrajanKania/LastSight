#pragma once

#include <array>
#include <cassert>
#include <vector>

#include "ls/asset/asset_buffer.hpp"
#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_types.hpp"

namespace ls::asset {

  class AssetRegistry {
  public:
    void registerAsset(AssetMetadata metadata) {
      assert(metadata.type != AssetType::Unknown && "[AssetRegistry] Cannot register asset with unknown type.");
      assetBuffers_[static_cast<std::size_t>(metadata.type)].add(metadata);
    }

    void unregisterAsset(DynamicAssetHandle handle) {
      if (!handle.isValid())
        return;
      assetBuffers_[static_cast<std::size_t>(handle.getType())].remove(handle.id);
    }

    AssetMetadata* getAssetMetadata(DynamicAssetHandle handle) {
      if (!handle.isValid())
        return nullptr;
      return assetBuffers_[static_cast<std::size_t>(handle.getType())].get(handle.id);
    }

    const AssetMetadata* getAssetMetadata(DynamicAssetHandle handle) const {
      if (!handle.isValid())
        return nullptr;
      return assetBuffers_[static_cast<std::size_t>(handle.getType())].get(handle.id);
    }

    const std::vector<AssetMetadata>& getAssetMetadataOfType(AssetType type) const {
      return assetBuffers_[static_cast<std::size_t>(type)].getAssets();
    }

    const std::array<AssetBuffer, kAssetTypeCount>& getAssetBuffers() const { return assetBuffers_; }

    void clear() {
      for (auto& assetBuffer : assetBuffers_) {
        assetBuffer.clear();
      }
    }

  private:
    std::array<AssetBuffer, kAssetTypeCount> assetBuffers_{};
  };

}  // namespace ls::asset
