#include "../shader.hpp"

#include <glad/gl.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "shader_exception.hpp"

namespace ls {

  Shader::Shader(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) {
    GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexShaderPath)};
    GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentShaderPath)};

    createProgram(vertexShader, fragmentShader, vertexShaderPath, fragmentShaderPath);

    cacheUniforms();
  }

  Shader::~Shader() {
    if (shaderProgram_)
      glDeleteProgram(shaderProgram_);
  }

  void Shader::use() noexcept { glUseProgram(shaderProgram_); }

  GLuint Shader::createShader(GLenum type, const std::filesystem::path& path) {
    std::string code{loadSource(path)};
    const char* source{code.c_str()};
    GLuint shader{compileShader(type, source, path)};
    return shader;
  }

  std::string Shader::loadSource(const std::filesystem::path& path) const {
    std::string code;

    std::fstream file(path, std::ios::in | std::ios::binary);
    if (file.is_open()) {
      std::stringstream sstream;
      sstream << file.rdbuf();
      code = sstream.str();

      file.close();
    } else {
      throw OpenFileError(path);
    }

    return code;
  }

  GLuint Shader::compileShader(GLenum type, const char* source, const std::filesystem::path& shaderPath) const {
    GLuint shader{glCreateShader(type)};
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success{0};
    std::array<char, 512> infoLog;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
      glDeleteShader(shader);

      throw CompileError(shaderPath, infoLog.data());
    }

    return shader;
  }

  void Shader::createProgram(GLuint vertexShader,
                             GLuint fragmentShader,
                             const std::filesystem::path& vertexShaderPath,
                             const std::filesystem::path& fragmentShaderPath) {
    if (!vertexShader || !fragmentShader)
      return;

    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vertexShader);
    glAttachShader(shaderProgram_, fragmentShader);

    glLinkProgram(shaderProgram_);

    int success{0};
    std::array<char, 512> infoLog;

    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram_, infoLog.size(), nullptr, infoLog.data());

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      glDeleteProgram(shaderProgram_);
      shaderProgram_ = 0;

      throw LinkError(vertexShaderPath, fragmentShaderPath, infoLog.data());
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  void Shader::cacheUniforms() {
    GLint uniformsCount{0};
    glGetProgramiv(shaderProgram_, GL_ACTIVE_UNIFORMS, &uniformsCount);

    if (uniformsCount == 0)
      return;

    GLint maxLength{0};
    glGetProgramiv(shaderProgram_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
    std::vector<char> nameBuffer(maxLength);
    for (GLint i{0}; i < uniformsCount; i++) {
      GLsizei uniformLength{0};

      glGetActiveUniform(shaderProgram_, i, nameBuffer.size(), &uniformLength, nullptr, nullptr, nameBuffer.data());

      std::string name(nameBuffer.data(), uniformLength);
      GLint uniformLocation{glGetUniformLocation(shaderProgram_, name.c_str())};
      uniformLocations_[name] = uniformLocation;
    }
  }

  GLint Shader::getUniformLocation(const std::string& name) const noexcept {
    if (auto it{uniformLocations_.find(name)}; it != uniformLocations_.end())
      return it->second;

    return -1;
  }

  void Shader::setBool(const std::string& name, GLboolean value) const noexcept {
    glUniform1i(getUniformLocation(name), value);
  }

  void Shader::setInt(const std::string& name, GLint value) const noexcept {
    glUniform1i(getUniformLocation(name), value);
  }

  void Shader::setFloat(const std::string& name, GLfloat value) const noexcept {
    glUniform1f(getUniformLocation(name), value);
  }

  void Shader::setVec3(const std::string& name, const glm::vec3& value) const noexcept {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
  }

  void Shader::setMat4(const std::string& name, const glm::mat4& value) const noexcept {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
  }

}  // namespace ls
