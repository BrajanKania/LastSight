#pragma once

#include <cstddef>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_manager.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/gfx/texture_2d.hpp"

namespace ls::gfx {

  class TextureManager {
  public:
    TextureManager() {};

    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    TextureManager(TextureManager&&) noexcept = default;
    TextureManager& operator=(TextureManager&&) noexcept = default;

    bool load(asset::TextureHandle handle, const asset::AssetManager& assetManager);
    void loadAll(const asset::AssetManager& assetManager);

    bool contains(asset::TextureHandle handle);
    const Texture2D* get(asset::TextureHandle handle) const;

    std::size_t getTextureCount() const { return textures_.size(); }
    void clear();

    void loadFromDir(const std::filesystem::path& path, bool recursive = false);

  private:
    std::vector<Texture2D> textures_;
    std::unordered_map<asset::AssetId, std::size_t> assetIdToIdx_{};
  };

}  // namespace ls::gfx
