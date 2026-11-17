#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string_view>
#include <utility>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::asset {

  using AssetId = std::uint64_t;
  inline constexpr AssetId kNullAsset{ 0 };

  ENUM()
  enum class AssetType : std::uint8_t {
    Unknown = 0,
    Texture2D,
    ShaderSource,
    Shader,
    Material,
    Prefab,
    Scene,
  };

  inline constexpr std::size_t kAssetTypeCount{ std::to_underlying(AssetType::Scene) + 1 };

  inline constexpr std::string_view kUnknownExtension{ "" };
  inline constexpr std::array<std::string_view, 2> kTexture2DExtensions{ ".jpg", ".png" };
  inline constexpr std::string_view kShaderSourceExtension{ ".glsl" };
  inline constexpr std::string_view kShaderExtension{ ".shader.json" };
  inline constexpr std::string_view kMaterialExtension{ ".material.json" };
  inline constexpr std::string_view kPrefabExtension{ ".prefab.json" };
  inline constexpr std::string_view kSceneExtension{ ".scene.json" };
  inline constexpr std::string_view kJsonExtension{ ".json" };
  inline constexpr std::string_view kCppExtension{ ".cpp" };
  inline constexpr std::string_view kHppExtension{ ".hpp" };

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
