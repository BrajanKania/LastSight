#include "ls/asset/asset_manager.hpp"

#include <filesystem>
#include <format>

#include "ls/asset/asset_context.hpp"
#include "ls/asset/asset_system.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/core/random_system.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/log_level.hpp"
#include "ls/serialization/asset_metadata_serializer.hpp"
#include "ls/serialization/serialization_context.hpp"

namespace ls::asset {

  bool AssetManager::loadAsset(const std::filesystem::path& path) { return loadAssetImpl(path, false); }

  bool AssetManager::loadAssetAsEngineAsset(const std::filesystem::path& path) { return loadAssetImpl(path, true); }

  bool AssetManager::loadFromDirectory(const std::filesystem::path& path, bool recursive) {
    return loadFromDirectoryImpl(path, recursive, false);
  }

  bool AssetManager::loadFromDirectoryAsEngineAsset(const std::filesystem::path& path, bool recursive) {
    return loadFromDirectoryImpl(path, recursive, true);
  }

  bool AssetManager::hasMetadata(const std::filesystem::path& path, bool isEngineAsset) const {
    return std::filesystem::exists(resolveMetadataPath(path, isEngineAsset));
  }

  AssetMetadata AssetManager::loadMetadata(const std::filesystem::path& path, bool isEngineAsset) {
    AssetMetadata metadata{};
    serialization::AssetMetadataSerializer serializer(serialization::SerializationContext{ ctx_ });
    if (!serializer.loadAssetMetadata(resolveMetadataPath(path, isEngineAsset), metadata)) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetManager] Failed to load AssetMetadata: {}", path.string()), debug::LogLevel::Warning
        );
      }
    }

    return metadata;
  }

  void AssetManager::saveMetadata(const AssetMetadata& metadata) {
    serialization::AssetMetadataSerializer serializer{ serialization::SerializationContext{ ctx_ } };
    if (!serializer.saveAssetMetadata(resolveMetadataPath(metadata.path, metadata.isEngineAsset), metadata)) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetManager] Failed to save AssetMetadata: {}", metadata.path.string()),
            debug::LogLevel::Error
        );
      }
    }
  }

  void AssetManager::saveAllMetadata() {
    for (const auto& assetBuffer : registry_.getAssetBuffers()) {
      for (const auto& assetMetadata : assetBuffer.getAssets()) {
        saveMetadata(assetMetadata);
      }
    }
  }

  bool AssetManager::loadAssetImpl(const std::filesystem::path& path, bool isEngineAsset) {
    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };

    if (!std::filesystem::exists(absPath) || !std::filesystem::is_regular_file(absPath)) {
      if (ctx_.console) {
        ctx_.console->log(std::format("[AssetManager] Cannot find path: {}", absPath.string()), debug::LogLevel::Error);
      }
      return false;
    }

    const auto relPath{ asset_system::rootRelativePath(absPath) };

    AssetMetadata metadata{};

    if (hasMetadata(absPath, isEngineAsset)) {
      metadata = loadMetadata(absPath, isEngineAsset);
      metadata.path = relPath;
    } else {
      metadata = AssetMetadata{
        .id = generateAssetId(),
        .type = asset_system::assetType(absPath),
        .path = relPath,
        .isEngineAsset = isEngineAsset,
      };
    }

    if (metadata.type != AssetType::Unknown) {
      registry_.registerAsset(metadata);
    }

    return true;
  }

  bool AssetManager::loadFromDirectoryImpl(const std::filesystem::path& path, bool recursive, bool isEngineDir) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
      if (ctx_.console) {
        ctx_.console->log(std::format("[AssetManager] Cannot find path: {}", path.string()), debug::LogLevel::Error);
      }
      return false;
    }

    auto processEntry{ [this](const std::filesystem::directory_entry& entry, bool isEngineAsset) {
      if (entry.is_regular_file()) {
        loadAssetImpl(asset_system::rootRelativePath(entry.path()), isEngineAsset);
      }
    } };

    if (recursive) {
      for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        processEntry(entry, isEngineDir);
      }
    } else {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        processEntry(entry, isEngineDir);
      }
    }

    return true;
  }

  AssetId AssetManager::generateAssetId() { return random_system::getRandom64(); }

  std::filesystem::path AssetManager::resolveMetadataPath(const std::filesystem::path& path, bool isEngineAsset) const {
    return isEngineAsset ? asset_system::resolveEngineMetadataPath(path) : asset_system::resolveGameMetadataPath(path);
  }

}  // namespace ls::asset
