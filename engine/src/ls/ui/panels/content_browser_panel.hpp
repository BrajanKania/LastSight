#pragma once

#include <filesystem>
#include <string>

#include "ls/asset/asset_system.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/ui/i_ui_panel.hpp"
#include "ls/ui/ui_context.hpp"

namespace ls::ui {

  class ContentBrowserPanel : public IUIPanel {
  public:
    ContentBrowserPanel() = default;

    virtual void render(const UIContext& ctx) override;

  private:
    void renderDirectoryList(const UIContext& ctx);
    void renderDirectoryListItem(const std::string& displayName, const std::filesystem::path& path);

    void renderContentGrid(const UIContext& ctx);
    void renderPathBreadcrumbs(const UIContext& ctx);

    void renderCreateFileModal(const UIContext& ctx);
    bool shouldOpenCreateFileModal_{ false };
    char filenamePathBuffer_[256]{};

    void renderCreateDirectoryModal(const UIContext& ctx);
    bool shouldOpenCreateDirectoryModal_{ false };
    char directoryPathBuffer_[256]{};

    void renderCreateAssetModal(const UIContext& ctx);
    bool shouldOpenCreateAssetModal_{ false };
    char assetNameBuffer_[256]{};
    asset::AssetType selectedAssetType_{ asset::AssetType::Unknown };

    void renderMovePathModal(const UIContext& ctx);
    bool shouldOpenMovePathModal_{ false };
    std::filesystem::path oldPath_{};
    char newPathBuffer_[256]{};

    void renderConvertModal(const UIContext& ctx);
    bool shouldOpenConvertModal_{ false };
    std::filesystem::path convertTargetPath_{};
    asset::AssetType selectedTargetAssetType_{ asset::AssetType::Unknown };

    std::filesystem::path currentPath_{ asset_system::rootRelativePath(asset_system::gameAssetsDir()) };
    char searchBuffer_[256]{};
    bool showExtensions_{ false };
  };

}  // namespace ls::ui
