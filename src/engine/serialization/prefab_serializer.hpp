#pragma once

#include <filesystem>

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/prefab/prefab_context.hpp"

namespace ls::serialization {

  class PrefabSerializer {
  public:
    explicit PrefabSerializer(prefab::PrefabContext ctx)
        : ctx_{ ctx } {}

    bool loadPrefab(const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId targetEntity) const;
    bool savePrefab(const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId sourceEntity) const;

  private:
    prefab::PrefabContext ctx_{};
  };

}  // namespace ls::serialization
