#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/ui/i_ui_panel.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  class UIManager {
  public:
    template <typename TPanel, typename... TArgs>
    void addPanel(const std::string& name, TArgs&&... args) {
      assert(nameToIndex_.find(name) == nameToIndex_.end() && "Panel name is already used.");
      panels_.push_back(std::make_unique<TPanel>(std::forward<TArgs>(args)...));
      nameToIndex_[name] = panels_.size() - 1;
    }

    void destroyPanel(const std::string& name) {
      auto itName{ nameToIndex_.find(name) };
      assert(itName != nameToIndex_.end() && "Attempted to destroy non-existent panel.");

      std::size_t index{ itName->second };
      panels_.erase(panels_.begin() + index);
      nameToIndex_.erase(itName);
      for (auto& [panelName, panelIndex] : nameToIndex_) {
        if (panelIndex > index) {
          --panelIndex;
        }
      }
    }

    IUIPanel& getPanel(const std::string& name) {
      auto itName{ nameToIndex_.find(name) };
      assert(itName != nameToIndex_.end() && "Attempted to get non-existent panel.");
      return *panels_[itName->second];
    }

    template <typename TPanel>
    TPanel& getPanel(const std::string& name) {
      IUIPanel& panel{ getPanel(name) };
      assert(dynamic_cast<TPanel*>(&panel) != nullptr && "Panel type mismatch.");

      return static_cast<TPanel&>(panel);
    }

    void render(const UIContext& ctx);

    void clear();

  private:
    std::unordered_map<std::string, std::size_t> nameToIndex_;
    std::vector<std::unique_ptr<IUIPanel>> panels_;
  };

}  // namespace ls::ui
