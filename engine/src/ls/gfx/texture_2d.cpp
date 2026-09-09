#include "ls/gfx/texture_2d.hpp"

#include <glad/gl.h>
#include <stb_image.h>

#include <stdexcept>

namespace ls::gfx {

  Texture2D::Texture2D(const std::filesystem::path& path) {
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data{ stbi_load(path.string().c_str(), &width_, &height_, &channels_, 0) };

    if (!data)
      throw std::runtime_error(std::format("Failed to load texture: \"{}\"", path.string()));

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLint format{ 0 };

    if (channels_ == 1)
      format = GL_RED;
    else if (channels_ == 3)
      format = GL_RGB;
    else if (channels_ == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture2D::Texture2D(int width, int height)
      : width_{ width },
        height_{ height } {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture2D::Texture2D(Texture2D&& other)
      : id_{ other.id_ },
        width_{ other.width_ },
        height_{ other.height_ },
        channels_{ other.channels_ } {
    other.id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
    other.channels_ = 0;
  }
  Texture2D& Texture2D::operator=(Texture2D&& other) {
    if (this != &other) {
      cleanup();

      id_ = other.id_;
      width_ = other.width_;
      height_ = other.height_;
      channels_ = other.channels_;

      other.id_ = 0;
      other.width_ = 0;
      other.height_ = 0;
      other.channels_ = 0;
    }
    return *this;
  }

  void Texture2D::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
  }

  void Texture2D::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

  void Texture2D::resize(int width, int height) {
    width_ = width;
    height_ = height;
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    unbind();
  }

  void Texture2D::cleanup() {
    if (id_) {
      glDeleteTextures(1, &id_);
      id_ = 0;
    }

    width_ = 0;
    height_ = 0;
    channels_ = 0;
  }

}  // namespace ls::gfx
