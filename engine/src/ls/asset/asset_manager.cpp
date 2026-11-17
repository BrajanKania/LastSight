#include "ls/asset/asset_manager.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <system_error>

#include "ls/asset/asset_context.hpp"
#include "ls/asset/asset_handle.hpp"
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
            std::format("[asset::AssetManager::loadMetadata] Failed to load AssetMetadata: {}", path.string()),
            debug::LogLevel::Warning
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
            std::format("[asset::AssetManager::saveMetadata] Failed to save AssetMetadata: {}", metadata.path.string()),
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

  bool AssetManager::createFile(const std::filesystem::path& path) {
    if (path.empty())
      return false;

    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };
    std::ofstream file(absPath);
    if (file.is_open()) {
      file.close();
      loadAsset(path);
      return true;
    }

    return false;
  }

  bool AssetManager::createDirectories(const std::filesystem::path& path) {
    if (path.empty())
      return false;

    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };
    std::filesystem::create_directories(absPath);
    return true;
  }

  bool AssetManager::createAsset(const std::filesystem::path& path, const std::string& name, asset::AssetType type) {
    if (path.empty() || name.empty() || type == asset::AssetType::Unknown)
      return false;

    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };
    const std::string extension{ asset_system::assetTypeExtension(type) };
    const std::string filename{ name + extension };
    std::filesystem::path fullPath{ absPath / filename };

    return createFile(fullPath);
  }

  bool AssetManager::removeAsset(DynamicAssetHandle handle) {
    if (!handle.isValid())
      return false;

    const AssetMetadata* assetMetadata{ registry_.getAssetMetadata(handle) };
    if (!assetMetadata || assetMetadata->path.empty())
      return false;

    std::filesystem::remove(asset_system::toAbsolutePath(assetMetadata->path));
    const std::filesystem::path metadataFilePath{
      resolveMetadataPath(assetMetadata->path, assetMetadata->isEngineAsset)
    };
    const std::filesystem::path absMetadataFilePath{ asset_system::toAbsolutePath(metadataFilePath) };
    if (!absMetadataFilePath.empty())
      std::filesystem::remove(absMetadataFilePath);

    registry_.unregisterAsset(handle);
    return true;
  }

  bool AssetManager::removePath(const std::filesystem::path& path) {
    if (path.empty())
      return false;

    const auto absPath{ asset_system::toAbsolutePath(path) };
    std::error_code ec{};

    if (!std::filesystem::exists(absPath, ec))
      return false;

    if (!std::filesystem::is_directory(absPath, ec)) {
      const auto handle{ findAssetHandleByPath(asset_system::rootRelativePath(absPath)) };
      return handle.isValid() ? removeAsset(handle) : std::filesystem::remove(absPath, ec);
    }

    std::vector<DynamicAssetHandle> handlesToUnregister{};
    for (const auto& entry : std::filesystem::recursive_directory_iterator(absPath, ec)) {
      if (!ec && entry.is_regular_file(ec)) {
        const auto handle{ findAssetHandleByPath(asset_system::rootRelativePath(entry.path())) };
        if (handle.isValid())
          handlesToUnregister.push_back(handle);
      }
    }

    std::filesystem::remove_all(absPath, ec);
    if (ec) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[asset::AssetManager::removePath] Error: {}", ec.message()), debug::LogLevel::Error
        );
      }
      return false;
    }

    for (const auto handle : handlesToUnregister)
      registry_.unregisterAsset(handle);

    const auto relGame{ asset_system::gameRelativePath(absPath) };
    if (!relGame.empty() && *relGame.begin() != "..") {
      std::filesystem::remove_all(asset_system::gameMetadataDir() / relGame, ec);
    } else {
      const auto relEngine{ asset_system::engineRelativePath(absPath) };
      if (!relEngine.empty() && *relEngine.begin() != "..") {
        std::filesystem::remove_all(asset_system::engineMetadataDir() / relEngine, ec);
      }
    }

    return true;
  }

  bool AssetManager::movePath(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) {
    if (oldPath.empty() || newPath.empty())
      return false;

    const auto absOldPath{ asset_system::toAbsolutePath(oldPath) };
    const auto absNewPath{ asset_system::toAbsolutePath(newPath) };

    if (absOldPath == absNewPath)
      return false;

    if (!std::filesystem::exists(absOldPath) || std::filesystem::exists(absNewPath))
      return false;

    std::error_code ec{};

    if (std::filesystem::is_directory(absOldPath)) {
      if (absNewPath.has_parent_path())
        createDirectories(absNewPath.parent_path());

      std::filesystem::rename(absOldPath, absNewPath, ec);
      if (ec) {
        ctx_.console->log(
            std::format("[asset::AssetManager::movePath] Error: {}", ec.message()), debug::LogLevel::Error
        );
        return false;
      }

      bool allSucceeded{ true };
      for (const auto& entry : std::filesystem::recursive_directory_iterator(absNewPath, ec)) {
        if (ec || !entry.is_regular_file(ec)) {
          continue;
        }

        const auto subPath{ entry.path().lexically_relative(absNewPath) };
        if (!moveMetadataPath(absOldPath / subPath, entry.path())) {
          allSucceeded = false;
        }
      }
      return allSucceeded;
    }

    return moveFilePath(absOldPath, absNewPath);
  }

  bool AssetManager::convertAssetType(const std::filesystem::path& path, const AssetType newType) {
    if (path.empty() || newType == AssetType::Unknown)
      return false;

    const auto absOldPath{ asset_system::toAbsolutePath(path) };
    if (!std::filesystem::exists(absOldPath) || !std::filesystem::is_regular_file(absOldPath))
      return false;

    const auto absNewPath{
      absOldPath.parent_path() /
      std::format("{}{}", asset_system::baseFilename(absOldPath), asset_system::assetTypeExtension(newType))
    };

    if (absOldPath == absNewPath || std::filesystem::exists(absNewPath))
      return false;

    const auto relOldPath{ asset_system::rootRelativePath(absOldPath) };
    const auto relNewPath{ asset_system::rootRelativePath(absNewPath) };
    const auto oldHandle{ findAssetHandleByPath(relOldPath) };

    std::error_code ec{};
    std::filesystem::rename(absOldPath, absNewPath, ec);
    if (ec) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetManager::convertAssetType] Error: {}", ec.message()), debug::LogLevel::Error
        );
      }
      return false;
    }

    if (oldHandle.isValid()) {
      const AssetMetadata* oldMetadata{ registry_.getAssetMetadata(oldHandle) };
      if (!oldMetadata)
        return false;

      AssetMetadata updatedMetadata{ *oldMetadata };
      updatedMetadata.type = newType;
      updatedMetadata.path = relNewPath;

      const auto oldMetadataPath{ resolveMetadataPath(relOldPath, updatedMetadata.isEngineAsset) };
      std::filesystem::remove(asset_system::toAbsolutePath(oldMetadataPath), ec);

      registry_.unregisterAsset(oldHandle);
      registry_.registerAsset(updatedMetadata);
      saveMetadata(updatedMetadata);
      return true;
    }

    const auto relToEngine{ asset_system::engineRelativePath(absNewPath) };
    const bool isEngineAsset{ !relToEngine.empty() && *relToEngine.begin() != ".." };
    return loadAssetImpl(relNewPath, isEngineAsset);
  }

  DynamicAssetHandle AssetManager::findAssetHandleByPath(const std::filesystem::path& path) {
    AssetType type{ asset_system::assetType(path) };
    if (type != AssetType::Unknown) {
      for (const auto& metadata : registry_.getAssetMetadataOfType(type)) {
        if (path == metadata.path) {
          return DynamicAssetHandle(metadata.id, type);
        }
      }
    }

    return DynamicAssetHandle{};
  }

  bool AssetManager::loadAssetImpl(const std::filesystem::path& path, bool isEngineAsset) {
    const std::filesystem::path absPath{ asset_system::toAbsolutePath(path) };

    if (!std::filesystem::exists(absPath) || !std::filesystem::is_regular_file(absPath)) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[asset::AssetManager::loadAssetImpl] Cannot find path: {}", absPath.string()),
            debug::LogLevel::Error
        );
      }
      return false;
    }

    const auto relPath{ asset_system::rootRelativePath(absPath) };

    AssetMetadata metadata{};

    bool needSave{ false };

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

      needSave = metadata.type != AssetType::Unknown;
    }

    if (metadata.type != AssetType::Unknown) {
      registry_.registerAsset(metadata);
    }

    if (needSave)
      saveMetadata(metadata);

    return true;
  }

  bool AssetManager::loadFromDirectoryImpl(const std::filesystem::path& path, bool recursive, bool isEngineDir) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[asset::AssetManager::loadFromDirectoryImpl] Cannot find path: {}", path.string()),
            debug::LogLevel::Error
        );
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

  bool AssetManager::moveFilePath(const std::filesystem::path& absOldPath, const std::filesystem::path& absNewPath) {
    std::error_code ec{};

    if (absNewPath.has_parent_path())
      createDirectories(absNewPath.parent_path());

    std::filesystem::rename(absOldPath, absNewPath, ec);
    if (ec) {
      ctx_.console->log(
          std::format("[asset::AssetManager::moveFilePath] Error: {}", ec.message()), debug::LogLevel::Error
      );
      return false;
    }

    return moveMetadataPath(absOldPath, absNewPath);
  }

  bool AssetManager::moveMetadataPath(
      const std::filesystem::path& absOldPath, const std::filesystem::path& absNewPath
  ) {
    const auto relOldPath{ asset_system::rootRelativePath(absOldPath) };
    const auto relNewPath{ asset_system::rootRelativePath(absNewPath) };

    auto handle{ findAssetHandleByPath(relOldPath) };
    auto* metadata{ registry_.getAssetMetadata(handle) };
    if (metadata != nullptr) {
      const auto oldMetadataPath{ resolveMetadataPath(relOldPath, metadata->isEngineAsset) };
      const auto newMetadataPath{ resolveMetadataPath(relNewPath, metadata->isEngineAsset) };

      const auto absOldMetadataPath{ asset_system::toAbsolutePath(oldMetadataPath) };
      const auto absNewMetadataPath{ asset_system::toAbsolutePath(newMetadataPath) };

      std::error_code ec{};
      if (absNewMetadataPath.has_parent_path())
        createDirectories(absNewMetadataPath.parent_path());

      std::filesystem::rename(resolveMetadataPath(relOldPath, metadata->isEngineAsset), newMetadataPath, ec);
      if (ec) {
        ctx_.console->log(
            std::format("[asset::AssetManager::moveMetadataPath] Error: {}", ec.message()), debug::LogLevel::Error
        );
        return false;
      }

      metadata->path = relNewPath;
      saveMetadata(*metadata);

      auto currentDir{ absOldMetadataPath.parent_path() };
      while (!currentDir.empty() && currentDir != currentDir.root_path()) {
        if (!std::filesystem::exists(currentDir, ec) || !std::filesystem::is_empty(currentDir, ec))
          break;

        std::filesystem::remove(currentDir, ec);
        if (ec)
          break;

        currentDir = currentDir.parent_path();
      }
    }

    return true;
  }

  AssetId AssetManager::generateAssetId() const { return random_system::getRandom64(); }

  std::filesystem::path AssetManager::resolveMetadataPath(const std::filesystem::path& path, bool isEngineAsset) const {
    return isEngineAsset ? asset_system::resolveEngineMetadataPath(path) : asset_system::resolveGameMetadataPath(path);
  }

}  // namespace ls::asset
