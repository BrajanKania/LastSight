#include "ls/ui/panels/content_browser_panel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <format>
#include <string>

#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_system.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/core/engine_context.hpp"
#include "ls/dispatch/event_queue.hpp"
#include "ls/events/request_convert_asset_type.hpp"
#include "ls/events/request_create_asset.hpp"
#include "ls/events/request_create_directories.hpp"
#include "ls/events/request_create_file.hpp"
#include "ls/events/request_move_path.hpp"
#include "ls/events/request_open_asset.hpp"
#include "ls/events/request_remove_asset.hpp"
#include "ls/events/request_remove_path.hpp"
#include "ls/gfx/texture_2d.hpp"
#include "ls/gfx/texture_manager.hpp"
#include "ls/gfx/texture_names.hpp"
#include "ls/ui/panels/panel_names.hpp"

namespace ls::ui {

  namespace {

    const gfx::Texture2D* getIcon(const std::filesystem::directory_entry& entry, const EngineContext& ctx) {
      if (entry.is_directory()) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kDirectory).as<asset::AssetType::Texture2D>()
        );
      }

      const std::string& filename{ entry.path().filename().string() };
      if (filename.ends_with(asset::kPrefabExtension)) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kPrefab).as<asset::AssetType::Texture2D>()
        );
      }

      if (std::ranges::find(asset::kTexture2DExtensions, entry.path().extension()) !=
          asset::kTexture2DExtensions.end()) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(entry.path()).as<asset::AssetType::Texture2D>()
        );
      }

      if (entry.path().extension().string().find(asset::kShaderSourceExtension) != std::string::npos) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kGLSL).as<asset::AssetType::Texture2D>()
        );
      }

      if (entry.path().extension().string().find(asset::kJsonExtension) != std::string::npos) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kJSON).as<asset::AssetType::Texture2D>()
        );
      }

      if (entry.path().extension().string().find(asset::kCppExtension) != std::string::npos) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kCpp).as<asset::AssetType::Texture2D>()
        );
      }

      if (entry.path().extension().string().find(asset::kHppExtension) != std::string::npos) {
        return ctx.textureManager->get(
            ctx.assetManager->findAssetHandleByPath(gfx::texture_name::kHpp).as<asset::AssetType::Texture2D>()
        );
      }

      return nullptr;
    }

    std::string getDisplayName(const std::filesystem::path& path, bool isDirectory, bool showExtensions) {
      if (showExtensions || isDirectory) {
        return path.filename().generic_string();
      }
      return asset_system::baseFilename(path);
    }

  }  // namespace

  void ContentBrowserPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "[ui::ContentBrowserPanel::render] Requires a valid EventQueue!");
    assert(
        ctx.engineCtx.textureManager != nullptr && "[ui::ContentBrowserPanel::render] Requires a valid TextureManager!"
    );

    renderCreateFileModal(ctx);
    renderCreateDirectoryModal(ctx);
    renderCreateAssetModal(ctx);
    renderMovePathModal(ctx);
    renderConvertModal(ctx);

    if (ImGui::Begin(ui::panel::kContentBrowser, &visible_, ImGuiWindowFlags_MenuBar)) {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("View")) {
          ImGui::MenuItem("Show File Extensions", nullptr, &showExtensions_);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      if (ImGui::BeginTable(
              "##ContentBrowserSplitter", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit
          )) {
        ImGui::TableSetupColumn("##DirectoryListColumn", ImGuiTableColumnFlags_WidthFixed, 200.f);
        ImGui::TableSetupColumn("##FileGridColumn", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        if (ImGui::BeginChild("DirectoryList")) {
          renderDirectoryList(ctx);
          ImGui::EndChild();
        }

        ImGui::TableNextColumn();
        if (ImGui::BeginChild("FileGrid")) {
          renderContentGrid(ctx);
          ImGui::EndChild();
        }

        ImGui::EndTable();
      }
    }
    ImGui::End();
  }

  void ContentBrowserPanel::renderDirectoryList(const UIContext& ctx) {
    ImGui::SeparatorText("Your Project");
    renderDirectoryListItem("Sources", asset_system::rootRelativePath(asset_system::gameSrcDir()));
    renderDirectoryListItem("Assets", asset_system::rootRelativePath(asset_system::gameAssetsDir()));

    ImGui::SeparatorText("Last Sight Engine");
    renderDirectoryListItem("Sources", asset_system::rootRelativePath(asset_system::engineSrcDir()));
    renderDirectoryListItem("Assets", asset_system::rootRelativePath(asset_system::engineAssetsDir()));
  }

  void ContentBrowserPanel::renderDirectoryListItem(const std::string& displayName, const std::filesystem::path& path) {
    std::string label{ std::format("{}##{}", displayName, path.generic_string()) };
    bool isDirectorySelected{ currentPath_.generic_string().starts_with(path.generic_string()) };
    if (isDirectorySelected) {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.6f, 1.f));
    }
    if (ImGui::Button(label.c_str(), ImVec2(-1.f, 20.f))) {
      currentPath_ = path;
    }
    if (isDirectorySelected) {
      ImGui::PopStyleColor(1);
    }
  }

  void ContentBrowserPanel::renderContentGrid(const UIContext& ctx) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(250.f);
    ImGui::InputTextWithHint("##ContentSearchBox", "Search something...", searchBuffer_, sizeof(searchBuffer_));

    ImGui::SameLine();
    renderPathBreadcrumbs(ctx);

    ImGui::Separator();
    ImGui::BeginChild("##ContentGridSection");
    if (std::filesystem::exists(currentPath_)) {
      const float availWidth{ ImGui::GetContentRegionAvail().x };

      const float iconSize{ 65.f };
      const float cardWidth{ 85.f };
      const float cardHeight{ 100.f };
      const float cardPaddingTop{ 4.f };
      const float cardSpacing{ 10.f };

      int columns{ static_cast<int>((availWidth + cardSpacing) / (cardWidth + cardSpacing)) };
      if (columns < 1)
        columns = 1;

      ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(cardSpacing * 0.5f, cardSpacing * 0.5f));

      if (ImGui::BeginTable("##ContentGridTable", columns, ImGuiTableFlags_SizingFixedSame)) {
        for (int i = 0; i < columns; ++i) {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, cardWidth);
        }

        const std::filesystem::path absCurrentPath{ asset_system::toAbsolutePath(currentPath_) };
        if (std::filesystem::exists(absCurrentPath)) {
          for (const auto& entry : std::filesystem::directory_iterator(absCurrentPath)) {
            const std::filesystem::path cardPath{ asset_system::rootRelativePath(entry.path()) };

            const bool isCardDirectory{ entry.is_directory() };

            const std::string filename{ getDisplayName(cardPath, isCardDirectory, showExtensions_) };
            if (searchBuffer_[0] != '\0' && filename.find(searchBuffer_) == std::string::npos) {
              continue;
            }

            ImGui::TableNextColumn();
            ImGui::PushID(cardPath.generic_string().c_str());

            asset::DynamicAssetHandle cardAssetHandle{};
            if (!isCardDirectory)
              cardAssetHandle = ctx.engineCtx.assetManager->findAssetHandleByPath(cardPath);
            const bool isCardAsset{ cardAssetHandle.isValid() };

            const bool isCardFile{ !isCardDirectory && !isCardAsset };

            const bool isCardSelected{ (isCardDirectory && ctx.selectionCtx->isSelectedDirectory() &&
                                        ctx.selectionCtx->selectedPath == cardPath) ||
                                       (isCardAsset && ctx.selectionCtx->isSelectedAsset() &&
                                        cardAssetHandle == ctx.selectionCtx->selectedAssetHandle) ||
                                       (isCardFile && ctx.selectionCtx->isSelectedFile() &&
                                        cardPath == ctx.selectionCtx->selectedPath) };

            const ImVec2 screenPos{ ImGui::GetCursorScreenPos() };
            const ImVec2 cardMin{ screenPos };
            const ImVec2 cardMax{ screenPos.x + cardWidth, screenPos.y + cardHeight };

            ImGui::InvisibleButton("##CardItem", ImVec2(cardWidth, cardHeight));

            const bool isCardHovered{ ImGui::IsItemHovered() };
            const bool isCardClicked{ ImGui::IsItemClicked(ImGuiMouseButton_Left) };
            const bool isCardDoubleClicked{ ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isCardHovered };

            if (isCardSelected && ImGui::BeginPopupContextItem("##CardContextMenu")) {
              if (isCardAsset) {
                if (ImGui::MenuItem("Open")) {
                  if (asset_system::isTextFileEditingSupported(entry.path())) {
                    ctx.engineCtx.eventQueue->publish(
                        event::RequestOpenAsset{
                            .handle = ctx.selectionCtx->selectedAssetHandle,
                        }
                    );
                  }
                }
                ImGui::Separator();
              }

              if (ImGui::MenuItem("Move")) {
                shouldOpenMovePathModal_ = true;
                oldPath_ = cardPath;
              }

              if (!isCardDirectory && ImGui::MenuItem("Convert")) {
                shouldOpenConvertModal_ = true;
                convertTargetPath_ = cardPath;
              }

              ImGui::Separator();

              if (ImGui::MenuItem("Remove")) {
                if (isCardAsset) {
                  ctx.engineCtx.eventQueue->publish(
                      event::RequestRemoveAsset{
                          .handle = ctx.selectionCtx->selectedAssetHandle,
                      }
                  );
                } else if (isCardDirectory || isCardFile) {
                  ctx.engineCtx.eventQueue->publish(
                      event::RequestRemovePath{
                          .path = cardPath,
                      }
                  );
                }
              }

              ImGui::EndPopup();
            }

            if (isCardHovered) {
              ImGui::SetTooltip("%s", filename.c_str());

              if (isCardClicked) {
                if (isCardDirectory)
                  ctx.selectionCtx->selectDirectory(cardPath);
                else if (isCardAsset)
                  ctx.selectionCtx->selectAsset(cardAssetHandle);
                else if (isCardFile)
                  ctx.selectionCtx->selectFile(cardPath);
              }

              if (isCardDoubleClicked) {
                if (isCardDirectory) {
                  currentPath_ /= entry.path().filename();
                } else if (isCardAsset && asset_system::isTextFileEditingSupported(cardPath)) {
                  ctx.engineCtx.eventQueue->publish(
                      event::RequestOpenAsset{
                          .handle = ctx.selectionCtx->selectedAssetHandle,
                      }
                  );
                }
              }
            }

            ImDrawList* drawList{ ImGui::GetWindowDrawList() };
            drawList->ChannelsSplit(2);
            drawList->ChannelsSetCurrent(1);

            const auto* icon{ getIcon(entry, ctx.engineCtx) };
            if (icon != nullptr && icon->getHeight() > 0) {
              const float width{ static_cast<float>(icon->getWidth()) };
              const float height{ static_cast<float>(icon->getHeight()) };
              const float aspectRatio{ width / height };

              ImVec2 renderSize{ iconSize, iconSize };
              if (aspectRatio > 1.0f) {
                renderSize = ImVec2(iconSize, iconSize / aspectRatio);
              } else {
                renderSize = ImVec2(iconSize * aspectRatio, iconSize);
              }

              const float offsetX{ (cardWidth - renderSize.x) * 0.5f };
              const float offsetY{ (iconSize - renderSize.y) * 0.5f + cardPaddingTop };

              ImGui::SetCursorScreenPos(ImVec2(screenPos.x + offsetX, screenPos.y + offsetY));
              ImGui::Image((ImTextureID)(uintptr_t)icon->getId(), renderSize, ImVec2(0, 1), ImVec2(1, 0));
            }

            const ImVec2 textMin{ screenPos.x + 4.0f, screenPos.y + iconSize + cardPaddingTop + 4.0f };
            const ImVec2 textMax{ screenPos.x + cardWidth - 4.0f, screenPos.y + cardHeight - 4.0f };
            ImGui::RenderTextClipped(textMin, textMax, filename.c_str(), nullptr, nullptr, ImVec2(0.5f, 0.f));

            drawList->ChannelsSetCurrent(0);
            ImU32 bgColor{ ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f)) };
            if (isCardHovered)
              bgColor = ImGui::GetColorU32(ImVec4(0.28f, 0.28f, 0.28f, 1.0f));
            if (isCardSelected)
              bgColor = ImGui::GetColorU32(ImVec4(0.20f, 0.45f, 0.50f, 1.0f));

            drawList->AddRectFilled(cardMin, cardMax, bgColor, 8.0f);

            if (isCardSelected) {
              drawList->AddRect(cardMin, cardMax, ImGui::GetColorU32(ImVec4(0.35f, 0.65f, 1.0f, 1.0f)), 8.0f, 0, 1.5f);
            }

            drawList->ChannelsMerge();

            ImGui::PopID();
          }
        }

        ImGui::EndTable();
      }

      ImGui::PopStyleVar();

    } else {
      ImGui::TextDisabled("Select something...");
    }

    if (ImGui::BeginPopupContextWindow(
            "HierarchyBlankContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems
        )) {
      if (ImGui::MenuItem("Create Asset")) {
        shouldOpenCreateAssetModal_ = true;
      }
      if (ImGui::MenuItem("Create Directory")) {
        shouldOpenCreateDirectoryModal_ = true;
      }
      if (ImGui::MenuItem("Create File")) {
        shouldOpenCreateFileModal_ = true;
      }
      ImGui::EndPopup();
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
      ctx.selectionCtx->clear();
    }

    ImGui::EndChild();
  }

  void ContentBrowserPanel::renderPathBreadcrumbs(const UIContext& ctx) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.15f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 0.25f));

    std::filesystem::path accumulatedPath{};
    bool isFirst{ true };
    bool pathChanged{ false };

    for (const auto& part : currentPath_) {
      accumulatedPath /= part;

      if (!isFirst) {
        ImGui::SameLine(0.f, 1.f);
        ImGui::TextDisabled("/");
        ImGui::SameLine(0.f, 1.f);
      }
      isFirst = false;

      const std::string partStr{ part.string() };
      if (partStr.empty() || partStr == "/")
        continue;

      ImGui::PushID(accumulatedPath.string().c_str());

      if (part == "game" || part == "engine") {
        ImGui::TextDisabled("%s", partStr.c_str());
      } else if (ImGui::SmallButton(partStr.c_str())) {
        currentPath_ = accumulatedPath;
        pathChanged = true;
      }

      ImGui::PopID();

      if (pathChanged) {
        break;
      }
    }

    ImGui::PopStyleColor(3);
  }

  void ContentBrowserPanel::renderCreateFileModal(const UIContext& ctx) {
    if (shouldOpenCreateFileModal_) {
      ImGui::OpenPopup("Create File");
      filenamePathBuffer_[0] = '\0';
      shouldOpenCreateFileModal_ = false;
    }

    if (ImGui::BeginPopupModal("Create File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::BeginChild("CreateFileFormSection", ImVec2(250.f, 40.f));
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint(
          "##InputFilePath", "Enter name or path...", filenamePathBuffer_, sizeof(filenamePathBuffer_)
      );
      ImGui::Separator();
      ImGui::EndChild();

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Create Button
        std::string pathStr{ filenamePathBuffer_ };
        bool isFilePathValid{ !pathStr.empty() };

        if (!isFilePathValid) {
          ImGui::BeginDisabled();
        }

        if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f))) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestCreateFile{
                  .path = currentPath_ / pathStr,
              }
          );
          ImGui::CloseCurrentPopup();
        }

        if (!isFilePathValid) {
          ImGui::EndDisabled();
        }
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void ContentBrowserPanel::renderCreateDirectoryModal(const UIContext& ctx) {
    if (shouldOpenCreateDirectoryModal_) {
      ImGui::OpenPopup("Create Directory");
      directoryPathBuffer_[0] = '\0';
      shouldOpenCreateDirectoryModal_ = false;
    }

    if (ImGui::BeginPopupModal("Create Directory", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::BeginChild("DirectoryPramsSection", ImVec2(250.f, 40.f));
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint(
          "##InputDirectoryPath", "Enter name / path...", directoryPathBuffer_, sizeof(directoryPathBuffer_)
      );
      ImGui::Separator();
      ImGui::EndChild();

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Create Button
        std::string pathStr{ directoryPathBuffer_ };
        bool isDirectoryPathValid{ !pathStr.empty() };

        if (!isDirectoryPathValid) {
          ImGui::BeginDisabled();
        }

        if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f))) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestCreateDirectories{
                  .path = currentPath_ / pathStr,
              }
          );
          ImGui::CloseCurrentPopup();
        }

        if (!isDirectoryPathValid) {
          ImGui::EndDisabled();
        }
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void ContentBrowserPanel::renderCreateAssetModal(const UIContext& ctx) {
    if (shouldOpenCreateAssetModal_) {
      ImGui::OpenPopup("Create Asset");
      assetNameBuffer_[0] = '\0';
      selectedAssetType_ = asset::AssetType::Unknown;
      shouldOpenCreateAssetModal_ = false;
    }

    if (ImGui::BeginPopupModal("Create Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      if (ImGui::BeginChild("##CreateAssetForm", ImVec2(250.f, 80.f))) {
        if (ImGui::BeginTable("##CreateAssetTable", 2, ImGuiTableFlags_Resizable)) {
          ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.2f);
          ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.8f);

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextDisabled("Name");
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.f);
          ImGui::InputTextWithHint("##AssetNameInput", "Enter name...", assetNameBuffer_, sizeof(assetNameBuffer_));

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextDisabled("Type");
          ImGui::TableNextColumn();

          const entt::meta_type assetTypeMeta{ entt::resolve<asset::AssetType>() };
          if (assetTypeMeta.is_enum()) {
            std::string currentEnumName{ "Unknown" };

            for (auto [enumDataId, enumData] : assetTypeMeta.data()) {
              entt::meta_any enumValueAny{ enumData.get({}) };
              if (enumValueAny == selectedAssetType_) {
                currentEnumName = enumData.name() ? enumData.name() : "Unknown";
                break;
              }
            }

            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::BeginCombo("##CreateAssetAssetTypeCombo", currentEnumName.c_str())) {
              for (auto [enumDataId, enumData] : assetTypeMeta.data()) {
                ImGui::PushID(static_cast<int>(enumDataId));
                entt::meta_any enumValueAny{ enumData.get({}) };
                const char* itemName{ enumData.name() ? enumData.name() : "Unnamed" };
                const bool isSelected{ enumValueAny == selectedAssetType_ };

                if (ImGui::Selectable(itemName, isSelected)) {
                  selectedAssetType_ = enumValueAny.cast<asset::AssetType>();
                }

                if (isSelected) {
                  ImGui::SetItemDefaultFocus();
                }
                ImGui::PopID();
              }
              ImGui::EndCombo();
            }
          }

          ImGui::EndTable();
        }

        ImGui::EndChild();
      }

      ImGui::Separator();

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      const std::string nameStr{ assetNameBuffer_ };
      bool isDataValid{ !nameStr.empty() && selectedAssetType_ != asset::AssetType::Unknown && !currentPath_.empty() };

      if (!isDataValid) {
        ImGui::BeginDisabled();
      }

      if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f))) {
        ctx.engineCtx.eventQueue->publish(
            event::RequestCreateAsset{
                .name = assetNameBuffer_,
                .type = selectedAssetType_,
                .path = currentPath_,
            }
        );
        ImGui::CloseCurrentPopup();
      }

      if (!isDataValid) {
        ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  void ContentBrowserPanel::renderMovePathModal(const UIContext& ctx) {
    if (shouldOpenMovePathModal_) {
      ImGui::OpenPopup("Move Path");
      const std::string oldPathStr{ oldPath_.generic_string() };
      std::snprintf(newPathBuffer_, sizeof(newPathBuffer_), "%s", oldPathStr.c_str());
      shouldOpenMovePathModal_ = false;
    }

    if (ImGui::BeginPopupModal("Move Path", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::BeginChild("PathPramsSection", ImVec2(350.f, 40.f));
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint("##InputPath", "Enter path...", newPathBuffer_, sizeof(newPathBuffer_));
      ImGui::Separator();
      ImGui::EndChild();

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Move Button
        std::string pathStr{ newPathBuffer_ };
        bool isPathValid{ !pathStr.empty() && pathStr != oldPath_ };

        if (!isPathValid) {
          ImGui::BeginDisabled();
        }

        if (ImGui::Button("Move", ImVec2(buttonWidth, 0.0f))) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestMovePath{
                  .oldPath = oldPath_,
                  .newPath = pathStr,
              }
          );
          ImGui::CloseCurrentPopup();
        }

        if (!isPathValid) {
          ImGui::EndDisabled();
        }
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void ContentBrowserPanel::renderConvertModal(const UIContext& ctx) {
    if (shouldOpenConvertModal_) {
      ImGui::OpenPopup("Convert");
      selectedTargetAssetType_ = asset::AssetType::Unknown;
      shouldOpenConvertModal_ = false;
    }

    if (ImGui::BeginPopupModal("Convert", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      if (ImGui::BeginChild("##ConvertForm", ImVec2(300.f, 80.f))) {
        if (ImGui::BeginTable("##ConvertTable", 2, ImGuiTableFlags_Resizable)) {
          ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.2f);
          ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.8f);

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextDisabled("File");
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1.f);
          const std::string fileName{ convertTargetPath_.filename().string() };
          ImGui::TextDisabled("%s", fileName.c_str());

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextDisabled("Type");
          ImGui::TableNextColumn();

          const entt::meta_type assetTypeMeta{ entt::resolve<asset::AssetType>() };
          if (assetTypeMeta.is_enum()) {
            std::string currentEnumName{ "Unknown" };

            for (auto [enumDataId, enumData] : assetTypeMeta.data()) {
              entt::meta_any enumValueAny{ enumData.get({}) };
              if (enumValueAny == selectedTargetAssetType_) {
                currentEnumName = enumData.name() ? enumData.name() : "Unknown";
                break;
              }
            }

            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::BeginCombo("##ConvertAssetTypeCombo", currentEnumName.c_str())) {
              for (auto [enumDataId, enumData] : assetTypeMeta.data()) {
                ImGui::PushID(static_cast<int>(enumDataId));
                entt::meta_any enumValueAny{ enumData.get({}) };
                const char* itemName{ enumData.name() ? enumData.name() : "Unnamed" };
                const bool isSelected{ enumValueAny == selectedTargetAssetType_ };

                if (ImGui::Selectable(itemName, isSelected)) {
                  selectedTargetAssetType_ = enumValueAny.cast<asset::AssetType>();
                }

                if (isSelected) {
                  ImGui::SetItemDefaultFocus();
                }
                ImGui::PopID();
              }
              ImGui::EndCombo();
            }
          }

          ImGui::EndTable();
        }

        ImGui::EndChild();
      }

      ImGui::Separator();

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      bool isDataValid{ !convertTargetPath_.empty() && selectedTargetAssetType_ != asset::AssetType::Unknown };

      if (!isDataValid) {
        ImGui::BeginDisabled();
      }

      if (ImGui::Button("Convert", ImVec2(buttonWidth, 0.0f))) {
        ctx.engineCtx.eventQueue->publish(
            event::RequestConvertAssetType{
                .path = convertTargetPath_,
                .type = selectedTargetAssetType_,
            }
        );
        ImGui::CloseCurrentPopup();
      }

      if (!isDataValid) {
        ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

}  // namespace ls::ui
