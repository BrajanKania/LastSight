#include "engine/renderer/renderer_system.hpp"

#include <glad/gl.h>

namespace ls::renderer_system {

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
  }  // namespace

  void setClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

  void clearColorBuffer() { glClear(GL_COLOR_BUFFER_BIT); }

  void drawArrays(unsigned int vao, Primitive primitive, unsigned int first, unsigned int count) {
    glBindVertexArray(vao);
    glDrawArrays(toGlPrimitive(primitive), first, count);
    glBindVertexArray(0);
  }

  void bindFramebuffer(unsigned int fbo) { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

}  // namespace ls::renderer_system
