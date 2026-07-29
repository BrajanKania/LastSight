#pragma once

namespace ls {

  class Framebuffer {
  public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    void cleanup();

    unsigned int getID() const { return fbo_; }
    unsigned int getColorBufferID() const { return colorTexture_; }

    void bind() const;
    void unBind() const;

    void resize(int width, int height);

  private:
    unsigned int fbo_{0};
    unsigned int colorTexture_{0};
    int width_{0};
    int height_{0};
  };

}  // namespace ls
