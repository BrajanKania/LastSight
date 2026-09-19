#pragma once

#include <filesystem>

#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/serialization/serialization_context.hpp"

namespace ls::serialization {

  class PrefabSerializer {
  public:
    explicit PrefabSerializer(serialization::SerializationContext ctx)
        : ctx_{ ctx } {}

    bool loadPrefab(const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId targetEntity) const;
    bool savePrefab(const std::filesystem::path& path, ecs::Registry& registry, ecs::EntityId sourceEntity) const;

  private:
    serialization::SerializationContext ctx_{};
  };

}  // namespace ls::serialization
