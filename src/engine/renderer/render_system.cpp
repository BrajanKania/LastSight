#include "engine/renderer/render_system.hpp"

#include <glad/gl.h>

#include <cstdint>

namespace ls::render_system {

  namespace {
    GLenum toGlPrimitive(Primitive primitive) {
      switch (primitive) {
        case Primitive::Triangle:
          return GL_TRIANGLES;
        case Primitive::Point:
          return GL_POINTS;
      }
      return 0;
    }

    glm::vec2 viewportSize{ 0.f, 0.f };

  }  // namespace

  void setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    viewportSize = glm::vec2(width, height);
  }

  glm::vec2 getViewportSize() { return viewportSize; }

  void setClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

  void clearColorBuffer() { glClear(GL_COLOR_BUFFER_BIT); }

  void drawArrays(uint32_t vao, Primitive primitive, uint32_t first, uint32_t count) {
    glBindVertexArray(vao);
    glDrawArrays(toGlPrimitive(primitive), first, count);
    glBindVertexArray(0);
  }

  void bindFramebuffer(uint32_t fbo) { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

  void bindShader(std::uint32_t shader) { glUseProgram(shader); }

}  // namespace ls::render_system
