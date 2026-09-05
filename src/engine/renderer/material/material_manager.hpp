#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "engine/renderer/material/material.hpp"
#include "engine/renderer/material/material_handle.hpp"

namespace ls::renderer {

  class MaterialManager {
  public:
    MaterialManager() = default;
    ~MaterialManager() = default;

    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;

    MaterialManager(MaterialManager&&) noexcept = default;
    MaterialManager& operator=(MaterialManager&&) noexcept = default;

    void create(
        const std::string& name,
        const std::filesystem::path& pathToVertexShader,
        const std::filesystem::path& pathToFragmentShader
    );

    bool contains(const std::string& name) const;
    bool contains(MaterialHandle handle) const;

    Material* get(MaterialHandle handle);
    const Material* get(MaterialHandle handle) const;

    Material* get(const std::string& name);
    const Material* get(const std::string& name) const;

    MaterialHandle getHandle(const std::string& name) const;
    const std::string& getName(MaterialHandle handle) const;
    const std::vector<std::string>& getNames() const { return names_; }

    std::size_t getMaterialCount() const { return materials_.size(); }
    void clear() {
      materials_.clear();
      names_.clear();
      nameToHandle_.clear();
    }

  private:
    std::vector<std::unique_ptr<Material>> materials_{};
    std::vector<std::string> names_{};
    std::unordered_map<std::string, MaterialHandle> nameToHandle_{};
  };

}  // namespace ls::renderer
