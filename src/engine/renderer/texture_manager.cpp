#include "engine/renderer/texture_manager.hpp"

#include <memory>

namespace ls {

  TextureManager::~TextureManager() {
    for (auto& texture : textures_)
      texture->cleanup();
  }

  uint32_t TextureManager::load(const std::string& name, const std::filesystem::path& path) {
    textures_.push_back(std::make_unique<Texture2D>(path));
    uint32_t id = textures_.size() - 1;
    nameToId_[name] = id;
    return id;
  }

  const Texture2D* TextureManager::get(uint32_t id) const {
    if (id < textures_.size())
      return textures_[id].get();

    return nullptr;
  }

  const Texture2D* TextureManager::get(const std::string& name) const {
    const auto it{ nameToId_.find(name) };

    if (it != nameToId_.end())
      return textures_[it->second].get();

    return nullptr;
  }

  uint32_t TextureManager::getId(const std::string& name) const {
    const auto it{ nameToId_.find(name) };
    if (it != nameToId_.end())
      return it->second;

    return 0;
  }
}  // namespace ls
