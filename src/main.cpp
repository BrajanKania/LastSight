#include <SDL3/SDL_log.h>
#include <glad/gl.h>

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

    glClearColor(0.f, 0.f, 0.f, 1.f);
    while (!window.shouldClose()) {
      window.pollEvents();

      glClear(GL_COLOR_BUFFER_BIT);

      shader.use();

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);

      window.swapBuffers();
    }

  } catch (const std::exception& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[\n%s\n]", e.what());
    return -1;
  }

  return 0;
}
