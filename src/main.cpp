#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <glad/gl.h>

#include "controller_system.hpp"
#include "renderer_system.hpp"
#include "shader.hpp"
#include "window.hpp"

int main() {
  try {
    ls::Window window(1000, 800);

    // clang-format off
    float vertex[] {
      -0.5f, -0.5f,
      0.5f, -0.5f,
      0.5f, 0.5f,

      -0.5f, -0.5f,
      0.5f, 0.5f,
      -0.5f, 0.5f
    };
    // clang-format on

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ls::Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    ls::renderer_system::setClearColor({0.2f, 0.2f, 0.2f, 1.f});
    while (!window.shouldClose()) {
      window.pollEvents();

      if (ls::controller_system::isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        window.close();
      }

      ls::renderer_system::useFramebuffer(0);

      ls::renderer_system::clearColorBuffer();
      shader.use();

      ls::renderer_system::drawArrays(vao, ls::renderer_system::Primitive::Triangle, 0, 6);

      window.swapBuffers();
    }

  } catch (const std::exception& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[\n%s\n]", e.what());
    return -1;
  }

  return 0;
}
