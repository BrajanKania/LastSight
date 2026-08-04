#pragma once
#include <SDL3/SDL_video.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

#include "engine/renderer/renderer_system.hpp"

namespace ls {

  class Window {
  public:
    Window(int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    bool shouldClose() const { return shouldClose_; }
    void close() { shouldClose_ = true; }

    void swapBuffers() { SDL_GL_SwapWindow(window_); }
    void pollEvents();

    bool wasResized() const { return wasResized_; }

    SDL_Window* getSDLWindow() { return window_; }

    SDL_GLContext getOpengGlContext() { return glContext_; }

  private:
    void resizeViewport() { renderer_system::setViewport(0, 0, width_, height_); }

  private:
    SDL_Window* window_{ nullptr };
    SDL_GLContext glContext_{ nullptr };

    int width_{ 0 };
    int height_{ 0 };
    bool shouldClose_{ false };
    bool wasResized_{ false };
  };

}  // namespace ls
