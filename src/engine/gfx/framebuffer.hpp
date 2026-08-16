#pragma once

#include <cstdint>

#include "engine/gfx/texture_2d.hpp"

namespace ls::gfx {

  class Framebuffer {
  public:
    explicit Framebuffer(int width = 0, int height = 0);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    uint32_t getId() const { return fbo_; }
    uint32_t getColorBufferId() const { return colorTexture_.getId(); }
    const Texture2D& getColorTexture() const { return colorTexture_; }

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    void bind() const;
    void unbind() const;

    void resize(int width, int height);

  private:
    void cleanup();

    uint32_t fbo_{ 0 };
    Texture2D colorTexture_;

    int width_{ 0 };
    int height_{ 0 };
  };

}  // namespace ls::gfx
