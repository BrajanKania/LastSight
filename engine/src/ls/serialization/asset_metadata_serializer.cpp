#include "ls/serialization/asset_metadata_serializer.hpp"

#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "ls/asset/asset_types.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/log_level.hpp"
#include "ls/serialization/serialization_context.hpp"
#include "ls/serialization/serialization_system.hpp"

namespace ls::serialization {

  bool AssetMetadataSerializer::loadAssetMetadata(
      const std::filesystem::path& path, asset::AssetMetadata& targetMetadata
  ) {
    std::ifstream file(path);
    if (!file.is_open()) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetMetadataSerializer] Failed to open file: {}", path.string()), debug::LogLevel::Error
        );
      }

      return false;
    }

    nlohmann::json metadataJson;
    try {
      file >> metadataJson;
    } catch (const nlohmann::json::parse_error& e) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetMetadataSerializer] Failed to parse file: {}, error: {}", path.string(), e.what()),
            debug::LogLevel::Error
        );
      }
    }

    entt::meta_any metadataInstance =
        serialization_system::deserializeReflected(entt::resolve<asset::AssetMetadata>(), metadataJson, ctx_);

    if (auto* metadata{ metadataInstance.try_cast<asset::AssetMetadata>() }) {
      targetMetadata = *metadata;
    } else {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetMetadataSerializer] Failed to create AssetMetadata, file: {}", path.string()),
            debug::LogLevel::Error
        );
      }
      return false;
    }

    return true;
  }

  bool AssetMetadataSerializer::saveAssetMetadata(
      const std::filesystem::path& path, const asset::AssetMetadata& sourceMetadata
  ) {
    if (path.has_parent_path()) {
      std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path);
    if (!file.is_open()) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[AssetMetadataSerializer] Failed to open file: {}", path.string()), debug::LogLevel::Error
        );
      }

      return false;
    }

    nlohmann::json metadataJson = serialization_system::serializeReflected(sourceMetadata, ctx_);

    file << metadataJson.dump(2);
    return true;
  }

}  // namespace ls::serialization
