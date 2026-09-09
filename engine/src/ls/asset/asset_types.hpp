#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <utility>

namespace ls::asset {

  using AssetId = std::uint64_t;
  inline constexpr AssetId kNullAsset{ 0 };

  enum class AssetType : std::uint8_t {
    Unknown = 0,
    Texture2D,
    Material,
    Prefab,
    Scene,
  };

  inline constexpr std::size_t kAssetTypeCount{ std::to_underlying(AssetType::Scene) + 1 };

  struct AssetMetadata {
    AssetId id{ kNullAsset };
    AssetType type{ AssetType::Unknown };
    std::filesystem::path path{};
  };

}  // namespace ls::asset
