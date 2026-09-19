#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <utility>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::asset {

  using AssetId = std::uint64_t;
  inline constexpr AssetId kNullAsset{ 0 };

  ENUM()
  enum class AssetType : std::uint8_t {
    Unknown = 0,
    Texture2D,
    Material,
    Prefab,
    Shader,
    Scene,
  };

  inline constexpr std::size_t kAssetTypeCount{ std::to_underlying(AssetType::Scene) + 1 };

  STRUCT()
  struct AssetMetadata {
    PROPERTY()
    AssetId id{ kNullAsset };

    PROPERTY()
    AssetType type{ AssetType::Unknown };

    PROPERTY()
    std::filesystem::path path{};

    PROPERTY()
    bool isEngineAsset{ false };
  };

}  // namespace ls::asset
