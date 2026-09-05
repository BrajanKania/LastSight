#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace ls::render_system {

  enum class Primitive { Triangle, Point };

  void setViewport(int x, int y, int width, int height);

  glm::vec2 getViewportSize();

  void setClearColor(const glm::vec4& color);

  void clearColorBuffer();

  void drawArrays(uint32_t vao, Primitive primitive, uint32_t first, uint32_t count);

  void bindFramebuffer(uint32_t fbo);

  void bindShader(std::uint32_t shader);

}  // namespace ls::render_system
