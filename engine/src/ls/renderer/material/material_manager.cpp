#include "ls/renderer/material/material_manager.hpp"

#include "ls/renderer/material/material.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::renderer {

  void MaterialManager::create(
      const std::string& name,
      const std::filesystem::path& pathToVertexShader,
      const std::filesystem::path& pathToFragmentShader
  ) {
    nameToHandle_[name] = MaterialHandle{ .id = static_cast<MaterialId>(materials_.size()) };
    materials_.push_back(std::make_unique<Material>(pathToVertexShader, pathToFragmentShader));
    names_.push_back(name);
  }

  bool MaterialManager::contains(const std::string& name) const {
    return nameToHandle_.find(name) != nameToHandle_.end();
  }

  bool MaterialManager::contains(MaterialHandle handle) const {
    return handle.isValid() && handle.id < materials_.size();
  }

  Material* MaterialManager::get(MaterialHandle handle) {
    if (contains(handle))
      return materials_[handle.id].get();

    return nullptr;
  }

  const Material* MaterialManager::get(MaterialHandle handle) const {
    if (contains(handle))
      return materials_[handle.id].get();

    return nullptr;
  }

  Material* MaterialManager::get(const std::string& name) {
    auto it{ nameToHandle_.find(name) };
    if (it != nameToHandle_.end() && contains(it->second))
      return materials_[it->second.id].get();

    return nullptr;
  }

  const Material* MaterialManager::get(const std::string& name) const {
    auto it{ nameToHandle_.find(name) };
    if (it != nameToHandle_.end() && contains(it->second))
      return materials_[it->second.id].get();

    return nullptr;
  }

  MaterialHandle MaterialManager::getHandle(const std::string& name) const {
    auto it{ nameToHandle_.find(name) };
    if (it != nameToHandle_.end())
      return it->second;

    return MaterialHandle{};
  }

  const std::string& MaterialManager::getName(MaterialHandle handle) const {
    static const std::string kEmpty{ "" };
    if (contains(handle))
      return names_[handle.id];

    return kEmpty;
  }

}  // namespace ls::renderer
