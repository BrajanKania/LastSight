#include "engine/serialization/scene_serializer.hpp"

#include <cassert>
#include <format>
#include <fstream>
#include <iostream>

#include "engine/components/entity_name.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/serialization/serialization_system.hpp"

namespace ls::serialization {

  bool SceneSerializer::loadScene(const std::filesystem::path& path) {
    assert(sceneCtx_.registry != nullptr && "[SceneSerializer] Requires a valid Registry!");
    assert(engineCtx_.textureManager != nullptr && "[SceneSerializer] Requires a valid TextureManager!");

    std::ifstream file(path);
    if (!file.is_open()) {
      std::cerr << std::format("[SceneSerializer] Failed to open scene file: {}\n", path.string());
      return false;
    }

    nlohmann::json sceneJson;
    try {
      file >> sceneJson;
    } catch (const nlohmann::json::parse_error& e) {
      std::cerr << std::format("[SceneSerializer]  Failed to parse file: {}. Error: {}\n", path.string(), e.what());
      return false;
    }

    if (!sceneJson.contains("entities") || !sceneJson["entities"].is_array()) {
      std::cerr << std::format(
          "[SceneSerializer] Incorrect scene format, lacking \"entities\" array, file: {}\n", path.string()
      );
      return false;
    }

    sceneCtx_.registry->clear();

    for (const auto& entityJson : sceneJson["entities"]) {
      auto entity{ sceneCtx_.registry->createEntity() };

      if (entityJson.contains("name") && entityJson["name"].is_string()) {
        std::string entityName = entityJson["name"].get<std::string>();
        sceneCtx_.registry->addComponent(entity, component::EntityName{ .name = entityName });
      }

      if (!entityJson.contains("components") || !entityJson["components"].is_object())
        continue;

      for (const auto& [componentName, componentJson] : entityJson["components"].items()) {
        if (componentName == "EntityName" && sceneCtx_.registry->hasComponent<component::EntityName>(entity)) {
          continue;
        }

        entt::meta_type componentType{ entt::resolve(entt::hashed_string{ componentName.c_str() }) };

        if (!componentType) {
          for (auto [id, type] : entt::resolve()) {
            if (type.name() && type.name() == componentName) {
              componentType = type;
              break;
            }
          }
        }

        if (!componentType) {
          std::cerr << std::format(
              "[SceneSerializer] Non valid component type: {}, file: {}\n", componentName, path.string()
          );
          continue;
        }

        entt::meta_any componentInstance;
        if (componentJson.is_null() || (componentJson.is_object() && componentJson.empty())) {
          componentInstance = componentType.construct();
        } else {
          componentInstance =
              serialization_system::deserializeReflected(componentType, componentJson, *engineCtx_.textureManager);
          if (!componentInstance) {
            componentInstance = componentType.construct();
          }
        }

        if (!componentInstance) {
          std::cerr << std::format(
              "[SceneSerializer] Failed to create component: {}, file: {}\n", componentName, path.string()
          );
          continue;
        }

        auto* set{ sceneCtx_.registry->getISparseSetByTypeId(componentType.id()) };
        if (set) {
          set->emplaceMeta(entity, componentInstance);
        } else {
          std::cerr << "[Serialization] Błąd: Brak SparseSet w rejestrze dla: " << componentName << "\n";
          std::cerr << std::format(
              "[SceneSerializer] Failed to get SparseSet for component: {}, file: {}\n", componentName, path.string()
          );
        }
      }
    }

    return true;
  }

  bool SceneSerializer::saveScene(const std::filesystem::path& path) {
    assert(sceneCtx_.registry != nullptr && "SceneSerializer requires a valid Registry!");
    assert(engineCtx_.textureManager != nullptr && "SceneSerializer requires a valid TextureManager!");

    nlohmann::json sceneJson = nlohmann::json::object();
    nlohmann::json entitiesArray = nlohmann::json::array();

    for (ecs::EntityId entity : sceneCtx_.registry->view<component::EntityName>()) {
      if (!sceneCtx_.registry->isValidEntity(entity))
        continue;

      nlohmann::json entityJson = nlohmann::json::object();
      nlohmann::json componentsJson = nlohmann::json::object();

      for (auto [typeId, type] : entt::resolve()) {
        auto* set{ sceneCtx_.registry->getISparseSetByTypeId(typeId) };
        if (!set)
          continue;

        void* rawComponent{ set->getRawComponent(entity) };
        if (!rawComponent)
          continue;

        entt::meta_any componentAny{ type.from_void(rawComponent) };

        const char* componentName{ type.name() ? type.name() : "Unknown component" };

        componentsJson[componentName] =
            serialization_system::serializeReflected(componentAny, *engineCtx_.textureManager);
      }

      entityJson["name"] = sceneCtx_.registry->getComponent<component::EntityName>(entity).name;
      entityJson["components"] = componentsJson;
      entitiesArray.push_back(entityJson);
    }

    sceneJson["entities"] = entitiesArray;

    if (path.has_parent_path()) {
      std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path);
    if (!file.is_open())
      return false;

    file << sceneJson.dump(2);
    return true;
  }

}  // namespace ls::serialization
