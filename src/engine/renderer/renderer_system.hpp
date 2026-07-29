#pragma once

#include <glm/glm.hpp>

namespace ls::renderer_system {

  enum class Primitive { Triangle, Point };

  void setViewport(int x, int y, int width, int height);

  glm::vec2 getViewportSize();

  void setClearColor(const glm::vec4& color);

  void clearColorBuffer();

  void drawArrays(unsigned int vao, Primitive primitive, unsigned int first, unsigned int count);

  void bindFramebuffer(unsigned int fbo);

}  // namespace ls::renderer_system
