#pragma once

#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ls/asset/asset_types.hpp"

namespace ls::asset {

  class AssetBuffer {
  public:
    void add(const AssetMetadata& metadata) {
      assert(
          idToIndex_.find(metadata.id) == idToIndex_.end() &&
          "[AssetBuffer] Attempted to add already exists AssetMetadata."
      );

      idToIndex_[metadata.id] = assets_.size();
      assets_.push_back(metadata);
    }

    void remove(AssetId id) {
      auto it{ idToIndex_.find(id) };
      if (it == idToIndex_.end())
        return;

      std::size_t indexToRemove{ it->second };
      std::size_t lastIndex{ idToIndex_.size() - 1 };

      if (indexToRemove != lastIndex) {
        assets_[indexToRemove] = std::move(assets_[lastIndex]);
        idToIndex_[assets_[lastIndex].id] = indexToRemove;
      }

      assets_.pop_back();
      idToIndex_.erase(it);
    }

    AssetMetadata* get(AssetId id) {
      auto it{ idToIndex_.find(id) };
      if (it == idToIndex_.end())
        return nullptr;

      return &assets_[it->second];
    }

    const AssetMetadata* get(AssetId id) const {
      auto it{ idToIndex_.find(id) };
      if (it == idToIndex_.end())
        return nullptr;

      return &assets_[it->second];
    }

    const std::vector<AssetMetadata>& getAssets() const { return assets_; }

    void clear() {
      assets_.clear();
      idToIndex_.clear();
    }

  private:
    std::vector<AssetMetadata> assets_{};
    std::unordered_map<AssetId, std::size_t> idToIndex_{};
  };

}  // namespace ls::asset
