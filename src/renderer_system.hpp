#pragma once

#include <glm/glm.hpp>

namespace ls {

namespace renderer_system {
enum class Primitive { Triangle, Point };

void setClearColor(const glm::vec4& color);

void clearColorBuffer();

void drawArrays(unsigned int vao, Primitive primitive, unsigned int first, unsigned int count);

void useFramebuffer(unsigned int fbo);

}  // namespace renderer_system

}  // namespace ls
