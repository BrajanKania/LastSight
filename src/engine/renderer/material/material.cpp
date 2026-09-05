#include "engine/renderer/material/material.hpp"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <variant>

#include "engine/renderer/render_system.hpp"

namespace ls::renderer {

  Material::Material(
      const std::filesystem::path& pathToVertexShader, const std::filesystem::path& pathToFragmentShader
  ) {
    shader_.emplace(pathToVertexShader, pathToFragmentShader);
  }

  void Material::bind() { shader_->use(); }

  void Material::unbind() { render_system::bindShader(0); }

  void Material::setProperty(const std::string& name, const MaterialPropertyValue& value) {
    auto it{ std::find_if(properties_.begin(), properties_.end(), [&](const MaterialProperty& prop) {
      return name == prop.name;
    }) };

    if (it != properties_.end()) {
      it->value = value;
    } else {
      properties_.push_back(
          MaterialProperty{
              .name = name,
              .value = value,
          }
      );
    }
  }

  void Material::applyProperties() {
    for (const auto& property : properties_) {
      if (property.name.empty())
        continue;

      applyProperty(property);
    }
  }

  void Material::applyProperty(const MaterialProperty& property) {
    if (property.name.empty())
      return;

    applyProperty(property.name, property.value);
    temporaryProperties_.push_back(property);
  }

  void Material::resetTemporaryProperties() {
    for (const auto& [name, value] : temporaryProperties_) {
      resetProperty(name, value);
    }
    temporaryProperties_.clear();
  }

  void Material::applyProperty(const std::string& name, const MaterialPropertyValue& value) {
    if (name.empty())
      return;

    if (auto* val{ std::get_if<bool>(&value) }) {
      shader_->setBool(name, *val);
    }
    if (auto* val{ std::get_if<int>(&value) }) {
      shader_->setInt(name, *val);
    }
    if (auto* val{ std::get_if<float>(&value) }) {
      shader_->setFloat(name, *val);
    }
    if (auto* val{ std::get_if<glm::vec2>(&value) }) {
      shader_->setVec2(name, *val);
    }
    if (auto* val{ std::get_if<glm::vec3>(&value) }) {
      shader_->setVec3(name, *val);
    }
    if (auto* val{ std::get_if<glm::vec4>(&value) }) {
      shader_->setVec4(name, *val);
    }
    if (auto* val{ std::get_if<glm::mat4>(&value) }) {
      shader_->setMat4(name, *val);
    }
  }

  void Material::resetProperty(const std::string& name, const MaterialPropertyValue& value) {
    if (std::holds_alternative<bool>(value)) {
      shader_->setBool(name, false);
    }
    if (std::holds_alternative<int>(value)) {
      shader_->setInt(name, 0);
    }
    if (std::holds_alternative<float>(value)) {
      shader_->setFloat(name, 0.f);
    }
    if (std::holds_alternative<glm::vec2>(value)) {
      shader_->setVec2(name, glm::vec2{ 0.f });
    }
    if (std::holds_alternative<glm::vec3>(value)) {
      shader_->setVec3(name, glm::vec3{ 0.f });
    }
    if (std::holds_alternative<glm::vec4>(value)) {
      shader_->setVec4(name, glm::vec4{ 0.f });
    }
    if (std::holds_alternative<glm::mat4>(value)) {
      shader_->setMat4(name, glm::mat4{ 1.f });
    }
  }

}  // namespace ls::renderer
