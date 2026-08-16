#include "engine/gfx/texture_manager.hpp"

#include <memory>

#include "engine/gfx/texture_handle.hpp"

namespace ls::gfx {

  bool TextureManager::contains(const std::string& name) const {
    return nameToHandle_.find(name) != nameToHandle_.end();
  }

  bool TextureManager::contains(TextureHandle handle) const {
    return (handle.id < textures_.size() && textures_[handle.id] != nullptr);
  }

  TextureHandle TextureManager::load(const std::string& name, const std::filesystem::path& path) {
    auto it{ nameToHandle_.find(name) };
    if (it != nameToHandle_.end()) {
      return it->second;
    }

    textures_.push_back(std::make_unique<Texture2D>(path));
    names_.push_back(name);
    uint32_t id = textures_.size() - 1;
    TextureHandle handle{ .id = id };
    nameToHandle_[name] = handle;
    return handle;
  }

  const Texture2D* TextureManager::get(TextureHandle handle) const {
    if (handle.id < textures_.size())
      return textures_[handle.id].get();

    return nullptr;
  }

  const Texture2D* TextureManager::get(const std::string& name) const {
    const auto it{ nameToHandle_.find(name) };

    if (it != nameToHandle_.end())
      return textures_[it->second.id].get();

    return nullptr;
  }

  TextureHandle TextureManager::getHandle(const std::string& name) const {
    const auto it{ nameToHandle_.find(name) };
    if (it != nameToHandle_.end())
      return it->second;

    return TextureHandle{};
  }

  const std::string& TextureManager::getName(TextureHandle handle) const {
    static const std::string kEmpty{ "" };
    if (!contains(handle)) {
      return kEmpty;
    }
    return names_[handle.id];
  }

  void TextureManager::clear() {
    textures_.clear();
    names_.clear();
    nameToHandle_.clear();

    textures_.push_back(nullptr);
    names_.push_back("");
  }

}  // namespace ls::gfx
