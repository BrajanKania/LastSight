#pragma once

#include <cstdint>
#include <filesystem>

#include "ls/asset/asset_handle.hpp"
#include "ls/ecs/types.hpp"

namespace ls::ui {

  enum class SelectionType : std::uint8_t {
    None,
    Entity,
    Asset,
    Directory,
    File,
  };

  struct SelectionContext {
    SelectionType type{ SelectionType::None };

    ecs::EntityId selectedEntity{ ecs::kNullEntity };
    asset::DynamicAssetHandle selectedAssetHandle{};
    std::filesystem::path selectedPath{};

    void selectEntity(ecs::EntityId entity) {
      clear();
      type = SelectionType::Entity;
      selectedEntity = entity;
    }

    void selectAsset(asset::DynamicAssetHandle handle) {
      clear();
      type = SelectionType::Asset;
      selectedAssetHandle = handle;
    }

    void selectDirectory(const std::filesystem::path& path) {
      clear();
      type = SelectionType::Directory;
      selectedPath = path;
    }

    void selectFile(const std::filesystem::path& path) {
      clear();
      type = SelectionType::File;
      selectedPath = path;
    }

    void clear() {
      type = SelectionType::None;
      selectedEntity = ecs::kNullEntity;
      selectedAssetHandle = asset::DynamicAssetHandle{};
      selectedPath.clear();
    }

    bool isSelectedEntity() const { return type == SelectionType::Entity && selectedEntity != ecs::kNullEntity; }
    bool isSelectedAsset() const { return type == SelectionType::Asset && selectedAssetHandle.isValid(); }
    bool isSelectedDirectory() const { return type == SelectionType::Directory && !selectedPath.empty(); }
    bool isSelectedFile() const { return type == SelectionType::File && !selectedPath.empty(); }
  };

}  // namespace ls::ui
