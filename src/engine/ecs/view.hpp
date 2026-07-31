#pragma once

#include <limits>
#include <vector>

#include "engine/ecs/sparse_set.hpp"

namespace ls::ecs {

  template <typename... Components>
  class View {
  public:
    class Iterator {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = Entity;
      using difference_type = std::ptrdiff_t;
      using pointer = const Entity*;
      using reference = Entity;

      Iterator(
          std::tuple<const SparseSet<Components>*...> sets,
          const std::vector<Entity>* smallestEntities,
          std::size_t index
      )
          : sets_{ sets },
            smallestEntities_{ smallestEntities },
            index_{ index } {
        advanceToValid();
      }

      Entity operator*() const { return (*smallestEntities_)[index_]; }

      Iterator& operator++() {
        ++index_;
        advanceToValid();
        return *this;
      }

      bool operator==(const Iterator& other) const {
        return index_ == other.index_ && smallestEntities_ == other.smallestEntities_;
      }

      bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
      void advanceToValid() {
        if (!smallestEntities_)
          return;

        while (index_ < smallestEntities_->size()) {
          Entity entity = (*smallestEntities_)[index_];
          if (isValid(entity)) {
            break;
          }
          ++index_;
        }
      }

      bool isValid(Entity entity) const {
        auto checkEntity = [entity](auto* set) { return set && set->hasComponent(entity); };
        return std::apply([&](auto*... s) { return (checkEntity(s) && ...); }, sets_);
      }

      std::tuple<const SparseSet<Components>*...> sets_;
      const std::vector<Entity>* smallestEntities_{ nullptr };
      std::size_t index_{ 0 };
    };

    explicit View(const SparseSet<Components>*... sets)
        : sets_{ sets... } {
      if (((sets == nullptr) || ...)) {
        smallestSetEntities_ = nullptr;
        return;
      }

      std::size_t minSize = std::numeric_limits<std::size_t>::max();
      auto findSmallest = [&](auto* set) {
        if (set->getEntities().size() < minSize) {
          minSize = set->getEntities().size();
          smallestSetEntities_ = &set->getEntities();
        }
      };
      (findSmallest(sets), ...);
    }

    Iterator begin() const {
      if (!smallestSetEntities_)
        return end();
      return Iterator(sets_, smallestSetEntities_, 0);
    }

    Iterator end() const {
      std::size_t endIdx = smallestSetEntities_ ? smallestSetEntities_->size() : 0;
      return Iterator(sets_, smallestSetEntities_, endIdx);
    }

    size_t size() const {
      if (smallestSetEntities_)
        return smallestSetEntities_->size();

      return 0;
    }

  private:
    std::tuple<const SparseSet<Components>*...> sets_;
    const std::vector<Entity>* smallestSetEntities_{ nullptr };
  };

}  // namespace ls::ecs
