#pragma once

#include <SDL3/SDL_filesystem.h>

#include <filesystem>

namespace ls::asset_system {

  namespace fs = std::filesystem;

  inline fs::path assetsDir() {
    static const fs::path path{fs::path{SDL_GetBasePath()} / "assets"};
    return path;
  }

  inline fs::path asset(const fs::path& relativePath) {
    fs::path path{assetsDir() / relativePath};

    if (!fs::exists(path))
      throw std::runtime_error("Asset not found: " + path.string());

    return path;
  }

  inline fs::path shader(const fs::path& name) { return asset(fs::path("shaders") / name); }

  inline fs::path texture(const fs::path& name) { return asset(fs::path("textures") / name); }

}  // namespace ls::asset_system
