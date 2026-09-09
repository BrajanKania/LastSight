#pragma once

#include <entt/core/fwd.hpp>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/prefab/prefab_context.hpp"
#include "ls/prefab/prefab_handle.hpp"

namespace ls::prefab {

  struct PrefabInfo {
    const std::string& name;
    const std::filesystem::path& path;
    const std::vector<entt::id_type>& componentTypeIds;
  };

  class PrefabManager {
  public:
    explicit PrefabManager(PrefabContext ctx)
        : ctx_{ ctx } {
      ctx_.prefabManager = this;
    }
    ~PrefabManager() = default;

    PrefabManager(const PrefabManager&) = delete;
    PrefabManager& operator=(const PrefabManager&) = delete;

    PrefabManager(PrefabManager&&) = delete;
    PrefabManager& operator=(PrefabManager&&) = delete;

    bool load(const std::filesystem::path& path);
    bool loadFromDir(const std::filesystem::path& path, bool recursive = false);

    bool save(const std::string& name, ecs::Registry& registry, ecs::EntityId entity);

    bool syncComponent(PrefabHandle handle, entt::id_type typeId, ecs::Registry& registry, ecs::EntityId entity);
    bool syncAllComponent(PrefabHandle handle, ecs::Registry& registry, ecs::EntityId entity);
    ecs::EntityId instantiate(PrefabHandle handle, ecs::Registry& registry);

    PrefabHandle getHandle(const std::string& name) const;
    std::vector<std::string> getNames() const;

    std::optional<PrefabInfo> getPrefabInfo(const PrefabHandle handle) const;

  private:
    struct PrefabData {
      PrefabId id{ 0 };
      std::string name{};
      std::filesystem::path path{};
      ecs::EntityId entity{ ecs::kNullEntity };
      std::vector<entt::id_type> componentTypeIds{};
    };

    PrefabData* getPrefabData(const PrefabHandle handle);
    const PrefabData* getPrefabData(const PrefabHandle handle) const;

    ecs::Registry prefabRegistry_{};

    std::vector<PrefabData> prefabs_{};
    std::unordered_map<std::string, PrefabHandle> nameToHandle_{};
    std::unordered_map<std::filesystem::path, PrefabHandle> pathToHandle_{};

    PrefabContext ctx_{};
  };

}  // namespace ls::prefab
