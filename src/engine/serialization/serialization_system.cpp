#include "engine/serialization/serialization_system.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <entt/meta/resolve.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <string>

#include "engine/core/engine_context.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/reflection/reflection_system.hpp"

namespace ls::serialization_system {

  nlohmann::json serializeReflected(entt::meta_any any, const gfx::TextureManager& textureManager) {
    if (!any)
      return nullptr;

    auto type{ any.type() };

    if (any.type() == entt::resolve<gfx::TextureHandle>()) {
      auto handle{ any.cast<gfx::TextureHandle>() };
      return textureManager.getName(handle);
    }

    if (auto* value{ any.try_cast<bool>() }) {
      return *value;
    }
    if (auto* value{ any.try_cast<int>() }) {
      return *value;
    }
    if (auto* value{ any.try_cast<uint32_t>() }) {
      return *value;
    }
    if (auto* value{ any.try_cast<std::size_t>() }) {
      return *value;
    }
    if (auto* value{ any.try_cast<float>() }) {
      return *value;
    }
    if (auto* str{ any.try_cast<std::string>() }) {
      return *str;
    }

    if (auto* vec{ any.try_cast<glm::vec2>() }) {
      return nlohmann::json::array({ vec->x, vec->y });
    }
    if (auto* vec{ any.try_cast<glm::vec3>() }) {
      return nlohmann::json::array({ vec->x, vec->y, vec->z });
    }
    if (auto* vec{ any.try_cast<glm::vec4>() }) {
      return nlohmann::json::array({ vec->x, vec->y, vec->z, vec->w });
    }

    if (any.type().is_enum()) {
      for (auto [id, data] : type.data()) {
        if (data.get({}) == any) {
          if (data.name()) {
            return data.name();
          }
          return static_cast<uint32_t>(id);
        }
      }
      return 0;
    }

    if (auto container{ any.as_sequence_container() }) {
      nlohmann::json jsonArray = nlohmann::json::array();
      for (auto element : container) {
        jsonArray.push_back(serializeReflected(element, textureManager));
      }
      return jsonArray;
    }

    nlohmann::json jsonObject = nlohmann::json::object();
    for (auto [id, data] : type.data()) {
      const ls::reflection_system::PropertyInfo* propInfo{ data.custom() };

      if (propInfo && propInfo->transient) {
        continue;
      }

      std::string fieldName{ data.name() ? data.name() : "Unnamed" };

      if (fieldName.empty()) {
        fieldName = std::to_string(static_cast<std::size_t>(id));
      }

      entt::meta_any fieldValue{ data.get(any) };
      jsonObject[fieldName] = serializeReflected(fieldValue, textureManager);
    }

    return jsonObject;
  }

  entt::meta_any deserializeReflected(
      entt::meta_type type, const nlohmann::json& jsonValue, const gfx::TextureManager& textureManager
  ) {
    if (jsonValue.is_null() || !type)
      return entt::meta_any{};

    if (type == entt::resolve<gfx::TextureHandle>()) {
      return jsonValue.is_string() ? textureManager.getHandle(jsonValue.get<std::string>()) : gfx::TextureHandle{};
    }
    if (type == entt::resolve<bool>()) {
      return jsonValue.is_boolean() ? jsonValue.get<bool>() : false;
    }
    if (type == entt::resolve<int>()) {
      return jsonValue.is_number() ? jsonValue.get<int>() : 0;
    }
    if (type == entt::resolve<uint32_t>()) {
      return jsonValue.is_number() ? jsonValue.get<uint32_t>() : 0u;
    }
    if (type == entt::resolve<std::size_t>()) {
      return jsonValue.is_number() ? jsonValue.get<std::size_t>() : static_cast<std::size_t>(0);
    }
    if (type == entt::resolve<float>()) {
      return jsonValue.is_number() ? jsonValue.get<float>() : 0.0f;
    }
    if (type == entt::resolve<std::string>()) {
      return jsonValue.is_string() ? jsonValue.get<std::string>() : std::string{};
    }

    if (type == entt::resolve<glm::vec2>()) {
      if (jsonValue.is_array() && jsonValue.size() >= 2) {
        return glm::vec2{ jsonValue[0].get<float>(), jsonValue[1].get<float>() };
      } else {
        return glm::vec2{ 0.f };
      }
    }
    if (type == entt::resolve<glm::vec3>()) {
      if (jsonValue.is_array() && jsonValue.size() >= 3) {
        return glm::vec3{ jsonValue[0].get<float>(), jsonValue[1].get<float>(), jsonValue[2].get<float>() };
      } else {
        return glm::vec3{ 0.f };
      }
    }
    if (type == entt::resolve<glm::vec4>()) {
      if (jsonValue.is_array() && jsonValue.size() >= 4) {
        return glm::vec4{
          jsonValue[0].get<float>(), jsonValue[1].get<float>(), jsonValue[2].get<float>(), jsonValue[3].get<float>()
        };
      } else {
        return glm::vec4{ 0.f };
      }
    }

    if (type.is_enum()) {
      if (jsonValue.is_string()) {
        std::string_view strVal{ jsonValue.get<std::string_view>() };
        for (auto [id, data] : type.data()) {
          if (data.name() && std::string_view(data.name()) == strVal) {
            return data.get({});
          }
        }
      }

      if (jsonValue.is_number()) {
        auto numVal = jsonValue.get<uint32_t>();
        for (auto [id, data] : type.data()) {
          entt::meta_any enumAny{ data.get({}) };

          if (enumAny) {
            if (auto* val{ enumAny.try_cast<uint32_t>() }) {
              if (*val == numVal)
                return enumAny;
            } else if (auto* val{ enumAny.try_cast<int>() }) {
              if (static_cast<uint32_t>(*val) == numVal)
                return enumAny;
            }
          }
        }
      }

      return {};
    }

    entt::meta_any instance{ type.construct() };
    if (!instance) {
      return {};
    }

    if (auto container{ instance.as_sequence_container() }) {
      if (jsonValue.is_array()) {
        entt::meta_type elementType{ container.value_type() };
        for (const auto& elementJson : jsonValue) {
          entt::meta_any elementValue{ deserializeReflected(elementType, elementJson, textureManager) };
          if (elementValue) {
            container.insert(container.end(), elementValue);
          }
        }
      }
      return instance;
    }

    if (!jsonValue.is_object())
      return instance;

    for (auto [dataId, data] : type.data()) {
      const ls::reflection_system::PropertyInfo* propInfo{ data.custom() };
      if (propInfo && propInfo->transient)
        continue;

      std::string memberName{ data.name() ? data.name() : "" };
      std::string displayName{ (propInfo && propInfo->displayName) ? propInfo->displayName : "" };

      const nlohmann::json* fieldJson{ nullptr };
      if (!memberName.empty() && jsonValue.contains(memberName)) {
        fieldJson = &jsonValue[memberName];
      } else if (!displayName.empty() && jsonValue.contains(displayName)) {
        fieldJson = &jsonValue[displayName];
      }

      if (fieldJson && !fieldJson->is_null()) {
        entt::meta_any fieldValue{ deserializeReflected(data.type(), *fieldJson, textureManager) };
        if (fieldValue) {
          data.set(instance, fieldValue);
        }
      }
    }

    return instance;
  }

}  // namespace ls::serialization_system
