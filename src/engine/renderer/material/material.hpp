#pragma once

#include <filesystem>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <vector>

#include "engine/gfx/shader.hpp"
#include "engine/renderer/material/material_property.hpp"

namespace ls::renderer {

  class Material {
  public:
    explicit Material(
        const std::filesystem::path& pathToVertexShader, const std::filesystem::path& pathToFragmentShader
    );

    void bind();
    void unbind();

    gfx::Shader& getShader() { return shader_.value(); }

    void setProperty(const std::string& name, const MaterialPropertyValue& value);

    void applyProperties();
    void applyProperty(const MaterialProperty& property);

    void resetTemporaryProperties();
    void clearProperties() { properties_.clear(); }

  private:
    void applyProperty(const std::string& name, const MaterialPropertyValue& value);
    void resetProperty(const std::string& name, const MaterialPropertyValue& value);

    std::optional<gfx::Shader> shader_;
    std::vector<MaterialProperty> properties_{};
    std::vector<MaterialProperty> temporaryProperties_{};
  };

}  // namespace ls::renderer
