#pragma once

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

#include <filesystem>
#include <string_view>

#include "ls/asset/asset_types.hpp"

namespace ls::asset_system {

  namespace fs = std::filesystem;

  inline fs::path rootDir() {
    static const fs::path path{ []() {
      const char* basePath{ SDL_GetBasePath() };
      return (!basePath ? fs::current_path() : fs::path{ basePath }).lexically_normal();
    }() };
    return path;
  }

  inline fs::path toAbsolutePath(const fs::path& path) {
    return (path.is_absolute() ? path : (rootDir() / path)).lexically_normal();
  }

  inline fs::path rootRelativePath(const fs::path& path) { return toAbsolutePath(path).lexically_relative(rootDir()); }

  inline fs::path gameDir() { return rootDir() / "game"; }
  inline fs::path engineDir() { return rootDir() / "engine"; }

  inline fs::path gameAssetsDir() { return gameDir() / "assets"; }
  inline fs::path engineAssetsDir() { return engineDir() / "assets"; }

  inline fs::path gameSrcDir() { return gameDir() / "src"; }
  inline fs::path engineSrcDir() { return engineDir() / "src/ls"; }

  inline fs::path gameMetadataDir() { return gameDir() / ".metadata"; }
  inline fs::path engineMetadataDir() { return engineDir() / ".metadata"; }

  inline fs::path gameRelativePath(const fs::path& path) { return toAbsolutePath(path).lexically_relative(gameDir()); }
  inline fs::path engineRelativePath(const fs::path& path) {
    return toAbsolutePath(path).lexically_relative(engineDir());
  }

  inline fs::path resolveEngineMetadataPath(const fs::path& path) {
    const fs::path relativeEnginePath{ engineRelativePath(path) };
    fs::path metadataPath{ engineMetadataDir() / relativeEnginePath };
    metadataPath += ".metadata.json";
    return metadataPath;
  }

  inline fs::path resolveGameMetadataPath(const fs::path& path) {
    const fs::path relativeGamePath{ gameRelativePath(path) };
    fs::path metadataPath{ gameMetadataDir() / relativeGamePath };
    metadataPath += ".metadata.json";
    return metadataPath;
  }

  inline bool isTexture2DExtension(const std::string& filename) {
    for (const auto& extension : asset::kTexture2DExtensions)
      if (filename.ends_with(extension))
        return true;
    return false;
  }

  inline asset::AssetType assetType(const fs::path& path) {
    const std::string filename{ path.filename().generic_string() };

    if (isTexture2DExtension(filename)) {
      return asset::AssetType::Texture2D;
    }
    if (filename.ends_with(asset::kShaderSourceExtension)) {
      return asset::AssetType::ShaderSource;
    }
    if (filename.ends_with(asset::kShaderExtension)) {
      return asset::AssetType::Shader;
    }
    if (filename.ends_with(asset::kMaterialExtension)) {
      return asset::AssetType::Material;
    }
    if (filename.ends_with(asset::kPrefabExtension)) {
      return asset::AssetType::Prefab;
    }
    if (filename.ends_with(asset::kSceneExtension)) {
      return asset::AssetType::Scene;
    }

    return asset::AssetType::Unknown;
  }

  inline std::string_view assetTypeExtension(asset::AssetType type) {
    switch (type) {
      case asset::AssetType::Unknown:
        return asset::kUnknownExtension;
      case asset::AssetType::Texture2D:
        return asset::kTexture2DExtensions[0];
      case asset::AssetType::ShaderSource:
        return asset::kShaderSourceExtension;
      case asset::AssetType::Shader:
        return asset::kShaderExtension;
      case asset::AssetType::Material:
        return asset::kMaterialExtension;
      case asset::AssetType::Prefab:
        return asset::kPrefabExtension;
      case asset::AssetType::Scene:
        return asset::kSceneExtension;
    }

    return "";
  }

  inline std::string baseFilename(const fs::path& path) {
    const std::string filename{ path.filename().generic_string() };
    const auto type{ assetType(path) };

    if (type != asset::AssetType::Unknown) {
      const std::string_view extension{ assetTypeExtension(type) };
      if (!extension.empty() && filename.ends_with(extension)) {
        return filename.substr(0, filename.size() - extension.size());
      }
    }

    const std::size_t firstDotIdx{ filename.find('.', 1) };
    return firstDotIdx != std::string::npos ? filename.substr(0, firstDotIdx) : filename;
  }

  inline bool isTextFileEditingSupported(const std::filesystem::path& path) {
    static constexpr std::array kSupportedFileExtensions{ ".json", ".glsl", ".cpp", ".hpp" };
    return std::ranges::find(kSupportedFileExtensions, path.filename().extension()) != kSupportedFileExtensions.end();
  }

  // deprecated ---
  inline fs::path asset(const fs::path& relativePath) {
    fs::path path{ gameAssetsDir() / relativePath };

    if (!fs::exists(path)) {
      throw std::runtime_error("Asset not found: " + path.string());
    }

    return path;
  }

  inline fs::path engineAsset(const fs::path& relativePath) {
    fs::path path{ engineAssetsDir() / relativePath };

    if (!fs::exists(path)) {
      throw std::runtime_error("Asset not found: " + path.string());
    }

    return path;
  }

  inline fs::path shader(const fs::path& name) { return asset(fs::path("shaders") / name); }

  inline fs::path engineShader(const fs::path& name) { return engineAsset(fs::path("shaders") / name); }

  inline fs::path scene(const fs::path& name) {
    return gameAssetsDir() / fs::path("scenes") / std::format("{}.json", name.string());
  }

  inline fs::path prefab(const fs::path& name) {
    return gameAssetsDir() / fs::path("prefabs") / std::format("{}.prefab.json", name.string());
  }

}  // namespace ls::asset_system
