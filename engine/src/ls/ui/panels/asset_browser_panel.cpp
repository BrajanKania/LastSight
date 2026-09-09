#include "ls/ui/panels/asset_browser_panel.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <string>

#include "ls/dispatch/event_queue.hpp"
#include "ls/events/request_open_asset.hpp"
#include "ls/gfx/texture_2d.hpp"
#include "ls/gfx/texture_manager.hpp"
#include "ls/gfx/texture_names.hpp"
#include "ls/ui/panels/panel_names.hpp"

namespace ls::ui {

  namespace {

    const gfx::Texture2D* getIcon(
        const std::filesystem::directory_entry& entry, const gfx::TextureManager& textureManager
    ) {
      if (entry.is_directory()) {
        return textureManager.get(gfx::texture_name::kDirectory);
      }

      const std::string& filename{ entry.path().filename().string() };
      if (filename.ends_with(".prefab.json")) {
        return textureManager.get(gfx::texture_name::kPrefab);
      }

      static constexpr std::array kSupportedTextureExtensions{ ".jpg", ".png" };
      if (std::ranges::find(kSupportedTextureExtensions, entry.path().extension()) !=
          kSupportedTextureExtensions.end()) {
        return textureManager.get(entry.path().filename().string());
      }

      if (entry.path().extension().string().find(".glsl") != std::string::npos) {
        return textureManager.get(gfx::texture_name::kGLSL);
      }

      if (entry.path().extension().string().find(".json") != std::string::npos) {
        return textureManager.get(gfx::texture_name::kJSON);
      }

      return nullptr;
    }

    bool isEditTextFileSupported(const std::filesystem::path& path) {
      static constexpr std::array kSupportedFileExtensions{ ".glsl", ".json" };
      return std::ranges::find(kSupportedFileExtensions, path.filename().extension()) != kSupportedFileExtensions.end();
    }

    std::string getDisplayName(const std::filesystem::path& path, bool showExtensions) {
      const std::string filename{ path.filename().string() };
      if (showExtensions) {
        return filename;
      }

      static constexpr std::string_view kPrefabExtension{ ".prefab.json" };
      if (filename.ends_with(kPrefabExtension)) {
        return filename.substr(0, filename.length() - kPrefabExtension.length());
      }

      return path.stem().string();
    }

  }  // namespace

  void AssetBrowserPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "[AssetBrowserPanel] Requires a valid EventQueue!");
    assert(ctx.engineCtx.textureManager != nullptr && "[AssetBrowserPanel] Requires a valid TextureManager!");

    if (ImGui::Begin(ui::panel::kAssetBrowser, &visible_, ImGuiWindowFlags_MenuBar)) {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("View")) {
          ImGui::MenuItem("Show File Extensions", nullptr, &showExtensions_);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      if (ImGui::Button("Reset##ResetPath")) {
        searchBuffer_[0] = '\0';
        currentPath_ = assetsPath_;
      }

      ImGui::SameLine();
      ImGui::SetNextItemWidth(200.f);
      ImGui::InputTextWithHint("##AssetFilter", "Search assets...", searchBuffer_, sizeof(searchBuffer_));

      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.15f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));

      std::filesystem::path accumulatedPath;
      bool isFirst = true;
      bool pathChanged = false;

      for (const auto& part : currentPath_) {
        accumulatedPath /= part;

        if (!isFirst) {
          ImGui::SameLine(0.0f, 2.0f);
          ImGui::TextDisabled("/");
          ImGui::SameLine(0.0f, 2.0f);
        }
        isFirst = false;

        const std::string partStr{ part.string() };
        if (partStr.empty() || partStr == "/")
          continue;

        ImGui::PushID(accumulatedPath.string().c_str());

        if (ImGui::SmallButton(partStr.c_str())) {
          currentPath_ = accumulatedPath;
          pathChanged = true;
        }

        ImGui::PopID();

        if (pathChanged) {
          break;
        }
      }

      ImGui::PopStyleColor(3);

      ImGui::Separator();

      if (std::filesystem::exists(currentPath_)) {
        const float availWidth{ ImGui::GetContentRegionAvail().x };

        const float iconSize{ 80.f };
        const float cardWidth{ 95.f };
        const float cardHeight{ 130.f };
        const float cardPaddingTop{ 4.f };

        int columns{ static_cast<int>(availWidth / (cardWidth + 12.f)) };
        if (columns < 1)
          columns = 1;

        if (ImGui::BeginTable("##AssetGridTable", columns)) {
          for (const auto& entry : std::filesystem::directory_iterator(currentPath_)) {
            const std::string filename{ getDisplayName(entry.path(), showExtensions_) };

            if (searchBuffer_[0] != '\0' && filename.find(searchBuffer_) == std::string::npos) {
              continue;
            }

            ImGui::TableNextColumn();
            ImGui::PushID(filename.c_str());

            const ImVec2 screenPos{ ImGui::GetCursorScreenPos() };
            const ImVec2 cardMin{ screenPos };
            const ImVec2 cardMax{ screenPos.x + cardWidth, screenPos.y + cardHeight };

            ImDrawList* drawList{ ImGui::GetWindowDrawList() };
            drawList->ChannelsSplit(2);
            drawList->ChannelsSetCurrent(1);

            ImGui::BeginGroup();

            const auto* icon{ getIcon(entry, *ctx.engineCtx.textureManager) };
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

              ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
              ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.f));
              ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.f));

              ImGui::ImageButton(
                  "##TileIcon",
                  (ImTextureID)(uintptr_t)icon->getId(),
                  renderSize,
                  ImVec2(0, 1),
                  ImVec2(1, 0),
                  ImVec4(0.f, 0.f, 0.f, 0.f),
                  ImVec4(1.f, 1.f, 1.f, 1.f)
              );

              ImGui::PopStyleColor(3);
              ImGui::PopStyleVar();
            }

            ImGui::SetCursorScreenPos(ImVec2(screenPos.x + 4.0f, screenPos.y + iconSize + cardPaddingTop + 4.f));
            ImGui::PushTextWrapPos(screenPos.x + cardWidth - 4.0f);
            ImGui::TextUnformatted(filename.c_str());
            ImGui::PopTextWrapPos();

            ImGui::SetCursorScreenPos(screenPos);
            ImGui::Dummy(ImVec2(cardWidth, cardHeight));

            ImGui::EndGroup();

            const bool isCardHovered{ ImGui::IsItemHovered() };
            if (isCardHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
              if (entry.is_directory()) {
                currentPath_ /= entry.path().filename();
              } else if (isEditTextFileSupported(entry)) {
                ctx.engineCtx.eventQueue->publish(
                    event::RequestOpenAsset{
                        .path = entry.path(),
                    }
                );
              }
            }

            drawList->ChannelsSetCurrent(0);
            const ImU32 bgColor{ isCardHovered ? ImGui::GetColorU32(ImVec4(0.28f, 0.28f, 0.28f, 1.0f))
                                               : ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f)) };

            drawList->AddRectFilled(cardMin, cardMax, bgColor, 8.0f);

            drawList->ChannelsMerge();

            ImGui::PopID();
          }

          ImGui::EndTable();
        }

      } else {
        ImGui::TextDisabled("Directory 'assets' not found.");
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
