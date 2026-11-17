#pragma once

#include "ls/asset/asset_types.hpp"

namespace ls::asset {

  template <AssetType Type = AssetType::Unknown>
  struct AssetHandle {
    AssetId id{ kNullAsset };

    static constexpr AssetType getType() { return Type; }

    bool isValid() const { return id != kNullAsset; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const AssetHandle&) const = default;
  };

  template <>
  struct AssetHandle<AssetType::Unknown> {
    AssetId id{ kNullAsset };
    AssetType type{ AssetType::Unknown };

    constexpr AssetHandle() = default;
    constexpr AssetHandle(AssetId id, AssetType type)
        : id(id),
          type(type) {}

    template <AssetType T>
    constexpr AssetHandle(const AssetHandle<T>& typedHandle)
        : id(typedHandle.id),
          type(T) {}

    AssetType getType() const { return type; }

    bool isValid() const { return id != kNullAsset && type != AssetType::Unknown; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const AssetHandle&) const = default;

    template <AssetType TargetType>
    AssetHandle<TargetType> as() const {
      if (type == TargetType || type == AssetType::Unknown) {
        return AssetHandle<TargetType>{ id };
      }
      return AssetHandle<TargetType>{ kNullAsset };
    }
  };

  using DynamicAssetHandle = AssetHandle<>;

  using TextureHandle = AssetHandle<AssetType::Texture2D>;
  using MaterialHandle = AssetHandle<AssetType::Material>;
  using PrefabHandle = AssetHandle<AssetType::Prefab>;
  using SceneHandle = AssetHandle<AssetType::Scene>;

}  // namespace ls::asset
