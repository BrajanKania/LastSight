#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ls/serialization/serialization_context.hpp"

namespace ls::serialization_system {

  nlohmann::json serializeReflected(entt::meta_any any, serialization::SerializationContext ctx);

  entt::meta_any deserializeReflected(
      entt::meta_type type, const nlohmann::json& jsonValue, serialization::SerializationContext ctx
  );

}  // namespace ls::serialization_system
