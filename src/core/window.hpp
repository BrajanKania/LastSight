#pragma once
#include <SDL3/SDL_video.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

namespace ls {

  class Window {
  public:
    Window(int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    glm::ivec2 size() const { return size_; }
    bool shouldClose() const { return shouldClose_; }
    void close() { shouldClose_ = true; }

    void swapBuffers() { SDL_GL_SwapWindow(window_); }
    void pollEvents();

  private:
    void resizeViewport() { glViewport(0, 0, size_.x, size_.y); }

  private:
    SDL_Window* window_{nullptr};
    SDL_GLContext glContext_{nullptr};

    glm::ivec2 size_{};
    bool shouldClose_{false};
  };

}  // namespace ls
