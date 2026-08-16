#pragma once

#include <SDL3/SDL_filesystem.h>

#include <filesystem>
#include <format>
#include <stdexcept>

namespace ls::asset_system {

  namespace fs = std::filesystem;

  inline fs::path assetsDir() {
    static const fs::path path{ []() {
      const char* basePath{ SDL_GetBasePath() };
      if (!basePath) {
        return fs::current_path() / "assets";
      }
      return fs::path{ basePath } / "assets";
    }() };
    return path;
  }

  inline fs::path resolve(const fs::path& relativePath) { return assetsDir() / relativePath; }

  inline fs::path asset(const fs::path& relativePath) {
    fs::path path{ resolve(relativePath) };

    if (!fs::exists(path)) {
      throw std::runtime_error("Asset not found: " + path.string());
    }

    return path;
  }

  inline fs::path shader(const fs::path& name) { return asset(fs::path("shaders") / name); }

  inline fs::path texture(const fs::path& name) { return asset(fs::path("textures") / name); }

  inline fs::path scene(const fs::path& name) {
    return resolve(fs::path("scenes") / std::format("{}.json", name.string()));
  }

}  // namespace ls::asset_system
