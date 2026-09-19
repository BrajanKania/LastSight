#pragma once

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

#include <filesystem>

#include "ls/asset/asset_types.hpp"

namespace ls::asset_system {

  namespace fs = std::filesystem;

  inline fs::path rootDir() {
    static const fs::path path{ []() {
      const char* basePath{ SDL_GetBasePath() };
      if (!basePath) {
        return fs::current_path();
      }
      return fs::path{ basePath };
    }() };
    return path;
  }

  inline fs::path toAbsolutePath(const fs::path& path) { return path.is_absolute() ? path : (rootDir() / path); }

  inline fs::path rootRelativePath(const fs::path& path) { return path.lexically_relative(rootDir()); }

  inline fs::path gameDir() { return rootDir() / "game"; }
  inline fs::path engineDir() { return rootDir() / "engine"; }

  inline fs::path gameAssetsDir() { return gameDir() / "assets"; }
  inline fs::path engineAssetsDir() { return engineDir() / "assets"; }

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

  inline asset::AssetType assetType(const fs::path& path) {
    const std::string filename{ path.filename().string() };

    if (filename.ends_with(".png") || filename.ends_with(".jpg")) {
      return asset::AssetType::Texture2D;
    }
    if (filename.ends_with(".material.json")) {
      return asset::AssetType::Material;
    }
    if (filename.ends_with(".prefab.json")) {
      return asset::AssetType::Prefab;
    }
    if (filename.ends_with(".scene.json")) {
      return asset::AssetType::Scene;
    }
    if (filename.ends_with(".glsl")) {
      return asset::AssetType::Shader;
    }

    return asset::AssetType::Unknown;
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
