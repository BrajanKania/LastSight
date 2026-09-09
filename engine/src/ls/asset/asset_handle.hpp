#pragma once

#include "ls/asset/asset_types.hpp"

namespace ls::asset {

  template <AssetType Type>
  struct AssetHandle {
    AssetId id{ 0 };

    constexpr AssetType getType() const { return Type; }

    bool isValid() const { return id != kNullAsset; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const AssetHandle&) const = default;
  };

  using TextureHandle = AssetHandle<AssetType::Texture2D>;
  using MaterialHandle = AssetHandle<AssetType::Material>;
  using PrefabHandle = AssetHandle<AssetType::Prefab>;
  using SceneHandle = AssetHandle<AssetType::Scene>;

}  // namespace ls::asset
