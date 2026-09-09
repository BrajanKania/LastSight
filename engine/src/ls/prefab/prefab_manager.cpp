#include "ls/prefab/prefab_manager.hpp"

#include <entt/core/fwd.hpp>
#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <format>
#include <optional>

#include "ls/components/prefab_instance.hpp"
#include "ls/core/asset_system.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/log_level.hpp"
#include "ls/ecs/types.hpp"
#include "ls/prefab/prefab_handle.hpp"
#include "ls/serialization/prefab_serializer.hpp"

namespace ls::prefab {

  namespace {

    std::string getFilename(const std::filesystem::path& path) {
      std::string fullFilename{ path.filename().string() };

      static constexpr std::string_view kPrefabExtension{ ".prefab.json" };
      if (fullFilename.ends_with(kPrefabExtension)) {
        return fullFilename.substr(0, fullFilename.length() - kPrefabExtension.length());
      }

      return fullFilename;
    }

    bool isSupportedExtension(const std::filesystem::path& path) {
      std::string fullFilename{ path.filename().string() };
      static constexpr std::string_view kPrefabExtension{ ".prefab.json" };
      return fullFilename.ends_with(kPrefabExtension);
    }

  }  // namespace

  bool PrefabManager::load(const std::filesystem::path& path) {
    std::string name{ getFilename(path) };
    auto itName{ nameToHandle_.find(name) };

    PrefabHandle handle{};
    PrefabData* prefabData{ nullptr };

    if (itName != nameToHandle_.end()) {
      handle = itName->second;
      prefabData = getPrefabData(handle);
      if (!prefabData)
        return false;

      if (prefabRegistry_.isValidEntity(prefabData->entity)) {
        prefabRegistry_.destroyEntity(prefabData->entity);
      }
    }

    ecs::EntityId entity{ prefabRegistry_.createEntity() };

    serialization::PrefabSerializer serializer(ctx_);
    if (!serializer.loadPrefab(path, prefabRegistry_, entity)) {
      prefabRegistry_.destroyEntity(entity);
      return false;
    }

    std::vector<entt::id_type> componentTypeIds{};
    for (auto [typeId, typeName] : entt::resolve()) {
      auto* set{ prefabRegistry_.getISparseSetByTypeId(typeId) };
      if (set && set->hasComponent(entity)) {
        componentTypeIds.push_back(typeId);
      }
    }

    if (prefabData) {
      pathToHandle_.erase(prefabData->path);
      prefabData->path = path;
      prefabData->entity = entity;
      prefabData->componentTypeIds = std::move(componentTypeIds);
      pathToHandle_.emplace(path, handle);
    } else {
      handle = PrefabHandle{ .id = static_cast<PrefabId>(prefabs_.size()) };

      prefabs_.push_back(
          PrefabData{
              .id = handle.id,
              .name = name,
              .path = path,
              .entity = entity,
              .componentTypeIds = std::move(componentTypeIds),
          }
      );

      pathToHandle_.emplace(path, handle);
      nameToHandle_.emplace(name, handle);
    }

    return true;
  }

  bool PrefabManager::loadFromDir(const std::filesystem::path& path, bool recursive) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
      if (ctx_.console) {
        ctx_.console->log(std::format("[PrefabManager] Cannot find path: {}", path.string()), debug::LogLevel::Error);
      }
      return false;
    }

    auto processEntry{ [this](const std::filesystem::directory_entry& entry) {
      if (entry.is_regular_file() && isSupportedExtension(entry.path())) {
        load(entry.path());
      }
    } };

    if (recursive) {
      for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        processEntry(entry);
      }
    } else {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        processEntry(entry);
      }
    }
    return true;
  }

  bool PrefabManager::save(const std::string& name, ecs::Registry& registry, ecs::EntityId entity) {
    serialization::PrefabSerializer serializer(ctx_);
    if (serializer.savePrefab(asset_system::prefab(name), registry, entity)) {
      if (load(asset_system::prefab(name))) {
        auto handle{ getHandle(name) };
        if (registry.hasComponent<component::PrefabInstance>(entity)) {
          registry.getComponent<component::PrefabInstance>(entity).handle = handle;
        } else {
          registry.addComponent(entity, component::PrefabInstance{ .handle = handle });
        }
        return true;
      }
    }
    return false;
  }

  bool PrefabManager::syncComponent(
      PrefabHandle handle, entt::id_type typeId, ecs::Registry& registry, ecs::EntityId entity
  ) {
    auto* prefabData{ getPrefabData(handle) };
    if (!prefabData) {
      return false;
    }

    auto prefabEntity{ prefabData->entity };
    if (!prefabRegistry_.isValidEntity(prefabEntity) || !registry.isValidEntity(entity)) {
      return false;
    }
    auto* prefabSet{ prefabRegistry_.getISparseSetByTypeId(typeId) };
    auto* targetSet{ registry.getISparseSetByTypeId(typeId) };

    if (!prefabSet || !targetSet)
      return false;

    void* rawSourceComponent{ prefabSet->getRawComponent(prefabEntity) };
    if (!rawSourceComponent)
      return false;

    entt::meta_type type{ entt::resolve(typeId) };
    if (!type)
      return false;

    entt::meta_any componentInstance{ type.from_void(rawSourceComponent) };

    targetSet->emplaceMeta(entity, componentInstance);
    return true;
  }

  bool PrefabManager::syncAllComponent(PrefabHandle handle, ecs::Registry& registry, ecs::EntityId entity) {
    auto* prefabData{ getPrefabData(handle) };
    if (!prefabData) {
      return false;
    }

    if (!prefabRegistry_.isValidEntity(prefabData->entity)) {
      return false;
    }

    for (auto typeId : prefabData->componentTypeIds) {
      if (!syncComponent(handle, typeId, registry, entity)) {
        return false;
      }
    }

    return true;
  }

  ecs::EntityId PrefabManager::instantiate(PrefabHandle handle, ecs::Registry& registry) {
    auto entity{ registry.createEntity() };

    if (!syncAllComponent(handle, registry, entity)) {
      registry.destroyEntity(entity);
      return ecs::kNullEntity;
    }

    registry.addComponent(entity, component::PrefabInstance{ .handle = handle });

    return entity;
  }

  PrefabHandle PrefabManager::getHandle(const std::string& name) const {
    auto itName{ nameToHandle_.find(name) };
    if (itName != nameToHandle_.end()) {
      return itName->second;
    }
    return PrefabHandle{};
  }

  std::vector<std::string> PrefabManager::getNames() const {
    std::vector<std::string> names{ prefabs_.size() };
    for (std::size_t i{ 0 }; i < names.size(); ++i) {
      names[i] = prefabs_[i].name;
    }

    return names;
  }
  std::optional<PrefabInfo> PrefabManager::getPrefabInfo(const PrefabHandle handle) const {
    const auto* prefabData{ getPrefabData(handle) };
    if (!prefabData)
      return std::nullopt;

    return PrefabInfo{
      .name = prefabData->name,
      .path = prefabData->path,
      .componentTypeIds = prefabData->componentTypeIds,
    };
  }

  PrefabManager::PrefabData* PrefabManager::getPrefabData(const PrefabHandle handle) {
    if (!handle.isValid() || handle.id >= prefabs_.size())
      return nullptr;

    return &prefabs_[handle.id];
  }

  const PrefabManager::PrefabData* PrefabManager::getPrefabData(const PrefabHandle handle) const {
    if (!handle.isValid() || handle.id >= prefabs_.size())
      return nullptr;

    return &prefabs_[handle.id];
  }

}  // namespace ls::prefab
