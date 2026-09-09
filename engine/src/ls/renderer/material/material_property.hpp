#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>
#include <variant>

namespace ls::renderer {

  using MaterialPropertyValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

  struct MaterialProperty {
    std::string name{};
    MaterialPropertyValue value{};
  };

}  // namespace ls::renderer
