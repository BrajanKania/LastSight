#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/gfx/texture_2d.hpp"

namespace ls::gfx {

  class TextureManager {
  public:
    ~TextureManager();

    uint32_t load(const std::string& name, const std::filesystem::path& path);

    const Texture2D* get(uint32_t id) const;

    const Texture2D* get(const std::string& name) const;

    uint32_t getId(const std::string& name) const;

    std::size_t getTextureCount() const { return textures_.size(); }

  private:
    std::vector<std::unique_ptr<Texture2D>> textures_;
    std::unordered_map<std::string, uint32_t> nameToId_{};
  };

}  // namespace ls::gfx
