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

    bool createFile(const std::filesystem::path& path);
    bool createDirectories(const std::filesystem::path& path);
    bool createAsset(const std::filesystem::path& path, const std::string& name, asset::AssetType type);

    bool removeAsset(DynamicAssetHandle handle);
    bool removePath(const std::filesystem::path& path);

    bool movePath(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

    bool convertAssetType(const std::filesystem::path& path, const AssetType newType);

    template <AssetType Type>
    AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) {
      return registry_.getAssetMetadata(handle);
    }

    template <AssetType Type>
    const AssetMetadata* getAssetMetadata(AssetHandle<Type> handle) const {
      return registry_.getAssetMetadata(handle);
    }

    AssetMetadata* getAssetMetadata(DynamicAssetHandle handle) { return registry_.getAssetMetadata(handle); }

    const AssetMetadata* getAssetMetadata(DynamicAssetHandle handle) const {
      return registry_.getAssetMetadata(handle);
    }

    const std::vector<AssetMetadata>& getAssetMetadataOfType(AssetType type) const {
      return registry_.getAssetMetadataOfType(type);
    }

    DynamicAssetHandle findAssetHandleByPath(const std::filesystem::path& path);

  private:
    bool loadAssetImpl(const std::filesystem::path& path, bool isEngineAsset);
    bool loadFromDirectoryImpl(const std::filesystem::path& path, bool recursive, bool isEngineDir);

    bool moveFilePath(const std::filesystem::path& absOldPath, const std::filesystem::path& absNewPath);
    bool moveMetadataPath(const std::filesystem::path& absOldPath, const std::filesystem::path& absNewPath);

    AssetId generateAssetId() const;
    std::filesystem::path resolveMetadataPath(const std::filesystem::path& path, bool isEngineAsset) const;

    AssetContext ctx_{};
    AssetRegistry registry_{};
  };

}  // namespace ls::asset
