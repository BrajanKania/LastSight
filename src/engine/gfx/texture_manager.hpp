#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/gfx/texture_2d.hpp"
#include "engine/gfx/texture_handle.hpp"

namespace ls::gfx {

  class TextureManager {
  public:
    TextureManager() {
      textures_.push_back(nullptr);
      names_.push_back("");
    };

    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    TextureManager(TextureManager&&) noexcept = default;
    TextureManager& operator=(TextureManager&&) noexcept = default;

    TextureHandle load(const std::string& name, const std::filesystem::path& path);

    bool contains(const std::string& name) const;
    bool contains(TextureHandle handle) const;

    const Texture2D* get(TextureHandle handle) const;
    const Texture2D* get(const std::string& name) const;

    TextureHandle getHandle(const std::string& name) const;
    const std::string& getName(TextureHandle handle) const;

    std::size_t getTextureCount() const { return textures_.size(); }
    void clear();

  private:
    std::vector<std::unique_ptr<Texture2D>> textures_;
    std::vector<std::string> names_;
    std::unordered_map<std::string, TextureHandle> nameToHandle_;
  };

}  // namespace ls::gfx
