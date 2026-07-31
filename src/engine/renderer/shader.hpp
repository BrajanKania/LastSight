#pragma once

#include <glad/gl.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace ls {

  class Shader {
  public:
    Shader(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() noexcept;

    GLuint getID() const noexcept { return shaderProgram_; }

    void setBool(const std::string& name, GLboolean value) const noexcept;
    void setInt(const std::string& name, GLint value) const noexcept;
    void setFloat(const std::string& name, GLfloat value) const noexcept;
    void setVec2(const std::string& name, const glm::vec2& value) const noexcept;
    void setVec3(const std::string& name, const glm::vec3& value) const noexcept;
    void setVec4(const std::string& name, const glm::vec4& value) const noexcept;
    void setMat4(const std::string& name, const glm::mat4& value) const noexcept;

  private:
    GLuint createShader(GLenum type, const std::filesystem::path& path);
    std::string loadSource(const std::filesystem::path& path) const;

    GLuint compileShader(GLenum type, const char* source, const std::filesystem::path& shaderPath) const;

    void createProgram(
        GLuint vertexShader,
        GLuint fragmentShader,
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath
    );

    void cacheUniforms();

    GLint getUniformLocation(const std::string& name) const noexcept;

    GLuint shaderProgram_{ 0 };
    std::unordered_map<std::string, GLint> uniformLocations_;
  };

}  // namespace ls
