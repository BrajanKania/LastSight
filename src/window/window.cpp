#include "window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <glad/gl.h>

#include <format>
#include <stdexcept>

namespace ls {

  Window::Window(int width, int height)
      : size_{width, height} {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      throw std::runtime_error(std::format("Failed to initialize SDL3: {}", SDL_GetError()));
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window_ = SDL_CreateWindow("Last sight", size_.x, size_.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_) {
      SDL_Quit();
      throw std::runtime_error(std::format("Failed to create window: {}", SDL_GetError()));
    }

    glContext_ = SDL_GL_CreateContext(window_);
    if (!glContext_) {
      SDL_DestroyWindow(window_);
      SDL_Quit();
      throw std::runtime_error(std::format("Failed to create OpenGL context: {}", SDL_GetError()));
    }

    if (!SDL_GL_MakeCurrent(window_, glContext_)) {
      SDL_GL_DestroyContext(glContext_);
      SDL_DestroyWindow(window_);
      SDL_Quit();
      throw std::runtime_error(std::format("Failed to make current OpenGL context: {}", SDL_GetError()));
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
      SDL_GL_DestroyContext(glContext_);
      SDL_DestroyWindow(window_);
      SDL_Quit();
      throw std::runtime_error("Failed to load OpenGL functions via GLAD");
    }

    SDL_GL_SetSwapInterval(1);

    resizeViewport();
  }

  Window::~Window() {
    if (glContext_)
      SDL_GL_DestroyContext(glContext_);

    if (window_)
      SDL_DestroyWindow(window_);

    SDL_Quit();
  }

  void Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        close();

      if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        int w, h;
        SDL_GetWindowSizeInPixels(window_, &w, &h);
        size_ = {w, h};
        resizeViewport();
      }
    }
  }

}  // namespace ls
