#pragma once

#include <filesystem>

#include "ls/asset/asset_context.hpp"
#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_registry.hpp"
#include "ls/asset/asset_types.hpp"

namespace ls::asset {

  class AssetManager {
  public:
    AssetManager(AssetContext ctx)
        : ctx_{ ctx } {}

    bool loadAsset(const std::filesystem::path& path);
    bool loadAssetAsEngineAsset(const std::filesystem::path& path);

    bool loadFromDirectory(const std::filesystem::path& path, bool recursive = true);
    bool loadFromDirectoryAsEngineAsset(const std::filesystem::path& path, bool recursive = true);

    bool hasMetadata(const std::filesystem::path& path, bool isEngineAsset) const;

    AssetMetadata loadMetadata(const std::filesystem::path& path, bool isEngineAsset);

    void saveMetadata(const AssetMetadata& metadata);
    void saveAllMetadata();

    template <AssetType Type>
    AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) {
      return registry_.getAssetMetadata(handle);
    }

    template <AssetType Type>
    const AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) const {
      return registry_.getAssetMetadata(handle);
    }

    const std::vector<AssetMetadata>& getAssetMetadataOfType(AssetType type) const {
      return registry_.getAssetMetadataOfType(type);
    }

    template <AssetType Type>
    AssetHandle<Type> findHandleByName(const std::string& name) const {
      for (const auto& metadata : registry_.getAssetMetadataOfType(Type)) {
        if (metadata.path.filename() == name)
          return AssetHandle<Type>{ metadata.id };
      }
      return AssetHandle<Type>{ kNullAsset };
    }

  private:
    bool loadAssetImpl(const std::filesystem::path& path, bool isEngineAsset);
    bool loadFromDirectoryImpl(const std::filesystem::path& path, bool recursive, bool isEngineDir);

    AssetId generateAssetId();
    std::filesystem::path resolveMetadataPath(const std::filesystem::path& path, bool isEngineAsset) const;

    AssetContext ctx_{};
    AssetRegistry registry_{};
  };

}  // namespace ls::asset
