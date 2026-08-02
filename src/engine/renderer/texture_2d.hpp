#pragma once

#include <filesystem>
namespace ls {

  class Texture2D {
  public:
    Texture2D(const std::filesystem::path& path);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    Texture2D(Texture2D&& other);
    Texture2D& operator=(Texture2D&& other);

    uint32_t getId() const { return id_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    void cleanup();

    void bind(uint32_t slot) const;
    void unbind() const;

  private:
    uint32_t id_{ 0 };
    int width_{ 0 };
    int height_{ 0 };
    int channels_{ 0 };
  };

}  // namespace ls
