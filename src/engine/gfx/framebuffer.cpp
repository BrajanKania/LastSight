#include "engine/gfx/framebuffer.hpp"

#include <glad/gl.h>

#include "engine/renderer/render_system.hpp"

namespace ls::gfx {

  Framebuffer::Framebuffer(int width, int height)
      : width_{ width },
        height_{ height },
        colorTexture_(width, height) {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_.getId(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  Framebuffer::~Framebuffer() { cleanup(); }

  Framebuffer::Framebuffer(Framebuffer&& other) noexcept
      : fbo_{ other.fbo_ },
        colorTexture_{ std::move(other.colorTexture_) },
        width_{ other.width_ },
        height_{ other.height_ } {
    other.fbo_ = 0;
    other.width_ = 0;
    other.height_ = 0;
  }

  Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
      cleanup();
      fbo_ = other.fbo_;
      colorTexture_ = std::move(other.colorTexture_);
      width_ = other.width_;
      height_ = other.height_;

      other.fbo_ = 0;
      other.width_ = 0;
      other.height_ = 0;
    }
    return *this;
  }

  void Framebuffer::bind() const { render_system::bindFramebuffer(fbo_); }
  void Framebuffer::unbind() const { render_system::bindFramebuffer(0); }

  void Framebuffer::resize(int width, int height) {
    width_ = width;
    height_ = height;
    colorTexture_.resize(width_, height_);
  }

  void Framebuffer::cleanup() {
    if (fbo_) {
      glDeleteFramebuffers(1, &fbo_);
      fbo_ = 0;
    }

    width_ = 0;
    height_ = 0;
  }

}  // namespace ls::gfx
