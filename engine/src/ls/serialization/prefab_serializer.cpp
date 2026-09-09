#include "ls/serialization/prefab_serializer.hpp"

#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "ls/components/prefab_instance.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/log_level.hpp"
#include "ls/serialization/serialization_system.hpp"

namespace ls::serialization {

  bool PrefabSerializer::loadPrefab(
      const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId targetEntity
  ) const {
    assert(ctx_.textureManager != nullptr && "[PrefabSerializer] Requires a valid TextureManager!");
    assert(ctx_.prefabManager != nullptr && "[PrefabSerializer] Requires a valid PrefabManager!");
    assert(ctx_.materialManager != nullptr && "[PrefabSerializer] Requires a valid MaterialManager!");

    std::ifstream file(path);
    if (!file.is_open()) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[PrefabSerializer] Failed to open prefab file: {}", path.string()), debug::LogLevel::Error
        );
      }
      return false;
    }

    nlohmann::json prefabJson;
    try {
      file >> prefabJson;
    } catch (const nlohmann::json::parse_error& e) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("Failed to parse file: {}. Error: {}", path.string(), e.what()), debug::LogLevel::Error
        );
      }
      return false;
    }

    if (!prefabJson.contains("components") || !prefabJson["components"].is_object()) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format(
                "[PrefabSerializer] Incorrect prefab format, lacking \"components\" object, file: {}", path.string()
            ),
            debug::LogLevel::Error
        );
      }
      return false;
    }

    for (const auto& [componentName, componentJson] : prefabJson["components"].items()) {
      entt::meta_type componentType{ entt::resolve(entt::hashed_string(componentName.c_str())) };

      if (!componentType) {
        for (auto [id, type] : entt::resolve()) {
          if (type.name() && type.name() == componentName) {
            componentType = type;
            break;
          }
        }
      }

      if (!componentType) {
        if (ctx_.console) {
          ctx_.console->log(
              std::format("[PrefabSerializer] Invalid component type: {}, file: {}\n", componentName, path.string()),
              debug::LogLevel::Error
          );
        }
        continue;
      }

      entt::meta_any componentInstance;
      if (componentJson.is_null() || (componentJson.is_object() && componentJson.empty())) {
        componentInstance = componentType.construct();
      } else {
        componentInstance = serialization_system::deserializeReflected(
            componentType, componentJson, *ctx_.materialManager, *ctx_.textureManager, *ctx_.prefabManager
        );
        if (!componentInstance) {
          componentInstance = componentType.construct();
        }
      }

      if (!componentInstance) {
        if (ctx_.console) {
          ctx_.console->log(
              std::format(
                  "[PrefabSerializer] Failed to create component: {}, file: {}\n", componentName, path.string()
              ),
              debug::LogLevel::Error
          );
        }
        continue;
      }

      auto* set{ registry.getISparseSetByTypeId(componentType.id()) };
      if (set) {
        set->emplaceMeta(targetEntity, componentInstance);
      } else {
        if (ctx_.console) {
          ctx_.console->log(
              std::format(
                  "[PrefabSerializer] Failed to get SparseSet for component: {}, file: {}\n",
                  componentName,
                  path.string()
              ),
              debug::LogLevel::Error
          );
        }
      }
    }

    return true;
  }

  bool PrefabSerializer::savePrefab(
      const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId sourceEntity
  ) const {
    assert(ctx_.textureManager != nullptr && "[PrefabSerializer] Requires a valid TextureManager!");
    assert(ctx_.materialManager != nullptr && "[PrefabSerializer] Requires a valid MaterialManager!");

    nlohmann::json entityJson;
    nlohmann::json componentsJson = nlohmann::json::object();

    for (auto [typeId, type] : entt::resolve()) {
      auto* set{ registry.getISparseSetByTypeId(typeId) };
      if (!set)
        continue;

      void* rawComponent{ set->getRawComponent(sourceEntity) };
      if (!rawComponent)
        continue;

      if (entt::type_id<component::PrefabInstance>().hash() == typeId)
        continue;

      entt::meta_any componentAny{ type.from_void(rawComponent) };

      const char* componentName{ type.name() ? type.name() : "Unknown component" };

      componentsJson[componentName] = serialization_system::serializeReflected(
          componentAny, *ctx_.materialManager, *ctx_.textureManager, *ctx_.prefabManager
      );
    }

    entityJson["components"] = componentsJson;

    if (path.has_parent_path()) {
      std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file{ path };

    if (!file.is_open()) {
      if (ctx_.console) {
        ctx_.console->log(
            std::format("[PrefabSerializer] Failed to open prefab file: {}", path.string()), debug::LogLevel::Error
        );
      }
      return false;
    }

    file << entityJson.dump(2);
    file.close();
    return true;
  }

}  // namespace ls::serialization
