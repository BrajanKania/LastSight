#include "engine/renderer/framebuffer.hpp"

#include <glad/gl.h>

#include "engine/renderer/renderer_system.hpp"

namespace ls {

  Framebuffer::Framebuffer(int width, int height)
      : width_{width},
        height_{height} {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(1, &colorTexture_);
    glBindTexture(GL_TEXTURE_2D, colorTexture_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  Framebuffer::~Framebuffer() { cleanup(); }

  Framebuffer::Framebuffer(Framebuffer&& other) noexcept
      : fbo_{other.fbo_},
        colorTexture_{other.colorTexture_},
        width_{other.width_},
        height_{other.height_} {
    other.fbo_ = 0;
    other.colorTexture_ = 0;
    other.width_ = 0;
    other.height_ = 0;
  }

  Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
      cleanup();
      fbo_ = other.fbo_;
      colorTexture_ = other.colorTexture_;
      width_ = other.width_;
      height_ = other.height_;

      other.fbo_ = 0;
      other.colorTexture_ = 0;
      other.width_ = 0;
      other.height_ = 0;
    }
    return *this;
  }

  void Framebuffer::cleanup() {
    if (fbo_) {
      glDeleteFramebuffers(1, &fbo_);
      fbo_ = 0;
    }

    if (colorTexture_) {
      glDeleteTextures(1, &colorTexture_);
      colorTexture_ = 0;
    }
  }

  void Framebuffer::bind() const { renderer_system::bindFramebuffer(fbo_); }
  void Framebuffer::unBind() const { renderer_system::bindFramebuffer(0); }

  void Framebuffer::resize(int width, int height) {
    width_ = width;
    height_ = height;
    glBindTexture(GL_TEXTURE_2D, colorTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}  // namespace ls
