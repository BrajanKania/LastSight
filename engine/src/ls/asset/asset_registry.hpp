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

    template <AssetType Type>
    void unregisterAsset(AssetHandle<Type> handle) {
      assetBuffers_[static_cast<std::size_t>(Type)].remove(handle.id);
    }

    template <AssetType Type>
    AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) {
      return assetBuffers_[static_cast<std::size_t>(Type)].get(handle.id);
    }

    template <AssetType Type>
    const AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) const {
      return assetBuffers_[static_cast<std::size_t>(Type)].get(handle.id);
    }

    const std::vector<AssetMetadata>& getAssetsOfType(AssetType type) const {
      return assetBuffers_[static_cast<std::size_t>(type)].getAssets();
    }

    void clear() {
      for (auto& assetBuffer : assetBuffers_) {
        assetBuffer.clear();
      }
    }

  private:
    std::array<AssetBuffer, kAssetTypeCount> assetBuffers_{};
  };

}  // namespace ls::asset
