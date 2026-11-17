#include "ls/gfx/texture_manager.hpp"

#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/gfx/texture_2d.hpp"

namespace ls::gfx {

  bool TextureManager::contains(asset::TextureHandle handle) {
    auto it{ assetIdToIdx_.find(handle.id) };
    return it != assetIdToIdx_.end() && it->second < textures_.size();
  }

  bool TextureManager::load(asset::TextureHandle handle, const asset::AssetManager& assetManager) {
    if (!handle.isValid())
      return false;

    auto* metadata{ assetManager.getAssetMetadata(handle) };
    if (!metadata)
      return false;

    auto it{ assetIdToIdx_.find(metadata->id) };
    if (it != assetIdToIdx_.end() && it->second < textures_.size())
      return true;

    textures_.push_back(Texture2D{ metadata->path });
    assetIdToIdx_[metadata->id] = textures_.size() - 1;
    return true;
  }

  void TextureManager::loadAll(const asset::AssetManager& assetManager) {
    for (const auto& metadata : assetManager.getAssetMetadataOfType(asset::TextureHandle::getType())) {
      load(asset::TextureHandle{ metadata.id }, assetManager);
    }
  }

  const Texture2D* TextureManager::get(asset::TextureHandle handle) const {
    auto it{ assetIdToIdx_.find(handle.id) };
    if (it != assetIdToIdx_.end())
      return &textures_[it->second];

    return nullptr;
  }

  void TextureManager::clear() {
    textures_.clear();
    assetIdToIdx_.clear();
  }

}  // namespace ls::gfx
