#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

namespace ls {

  namespace ecs {

    using Entity = uint32_t;
    constexpr Entity kNullEntity{std::numeric_limits<Entity>::max()};
    using ComponentType = uint16_t;

    class ISparseSet {
    public:
      virtual ~ISparseSet() = default;
      virtual void destroyComponent(Entity entity) = 0;
    };

    template <typename Component>
    class SparseSet : public ISparseSet {
    public:
      void destroyComponent(Entity entity) override {
        if (!hasComponent(entity))
          return;

        auto entityOfLastElement{dense_.back()};
        auto indexOfRemoved{sparse_[entity]};

        dense_[indexOfRemoved] = entityOfLastElement;
        components_[indexOfRemoved] = std::move(components_.back());
        sparse_[entityOfLastElement] = indexOfRemoved;

        components_.pop_back();
        dense_.pop_back();
        sparse_[entity] = kNullEntity;
      }

      void addComponent(Entity entity, const Component& component) {
        if (sparse_.size() <= entity) {
          sparse_.resize(entity + 1, kNullEntity);
        }

        sparse_[entity] = dense_.size();
        dense_.push_back(entity);
        components_.push_back(component);
      }

      bool hasComponent(Entity entity) const { return (entity < sparse_.size() && sparse_[entity] != kNullEntity); }

      Component& get(Entity entity) { return components_[sparse_[entity]]; }

      const Component& get(Entity entity) const { return components_[sparse_[entity]]; }

      const std::vector<Entity>& getEntities() const { return dense_; }

    private:
      std::vector<Entity> sparse_{};
      std::vector<Entity> dense_{};
      std::vector<Component> components_{};
    };

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

        Iterator(std::tuple<const SparseSet<Components>*...> sets,
                 const std::vector<Entity>* smallestEntities,
                 std::size_t index)
            : sets_{sets},
              smallestEntities_{smallestEntities},
              index_{index} {
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
        const std::vector<Entity>* smallestEntities_{nullptr};
        std::size_t index_{0};
      };

      explicit View(const SparseSet<Components>*... sets)
          : sets_{sets...} {
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

    private:
      std::tuple<const SparseSet<Components>*...> sets_;
      const std::vector<Entity>* smallestSetEntities_{nullptr};
    };

    class Registry {
    public:
      Entity createEntity() {
        Entity entity;
        if (!availableEntities_.empty()) {
          entity = availableEntities_.back();
          availableEntities_.pop_back();
        } else {
          entity = nextEntity_++;
        }
        return entity;
      }

      void destroyEntity(Entity entity) {
        availableEntities_.push_back(entity);
        for (auto& sparseSet : sparseSets_) {
          if (sparseSet)
            sparseSet->destroyComponent(entity);
        }
      }

      template <typename T>
      bool hasComponent(Entity entity) const {
        return getSparseSetPointer<T>()->hasComponent(entity);
      }

      template <typename T>
      void addComponent(Entity entity, T component) {
        ComponentType componentTypeId{getComponentTypeId<T>()};
        if (sparseSets_.size() <= componentTypeId) {
          registerComponent<T>();
        }

        getSparseSetPointer<T>()->addComponent(entity, component);
      }

      template <typename T>
      void destroyComponent(Entity entity) {
        ComponentType componentTypeId{getComponentTypeId<T>()};
        if (sparseSets_.size() <= componentTypeId) {
          registerComponent<T>();
        }

        getSparseSetPointer<T>()->destroyComponent(entity);
      }

      template <typename T>
      T& getComponent(Entity entity) {
        return getSparseSetPointer<T>()->get(entity);
      }

      template <typename T>
      const T& getComponent(Entity entity) const {
        return getSparseSetPointer<T>()->get(entity);
      }

      template <typename... Components>
      auto view() const {
        return View<Components...>(getSparseSetPointer<Components>()...);
      }

    private:
      template <typename T>
      static ComponentType getComponentTypeId() {
        static ComponentType id{nextComponentType_++};
        return id;
      }

      template <typename T>
      void registerComponent() {
        ComponentType componentTypeId{getComponentTypeId<T>()};
        if (sparseSets_.size() <= componentTypeId) {
          sparseSets_.resize(componentTypeId + 1);
        }
        sparseSets_[componentTypeId] = std::make_unique<SparseSet<T>>();
      }

      template <typename T>
      SparseSet<T>* getSparseSetPointer() {
        return static_cast<SparseSet<T>*>(sparseSets_[getComponentTypeId<T>()].get());
      }

      template <typename T>
      const SparseSet<T>* getSparseSetPointer() const {
        return static_cast<const SparseSet<T>*>(sparseSets_[getComponentTypeId<T>()].get());
      }

      Entity nextEntity_{0};
      std::vector<Entity> availableEntities_{};

      inline static ComponentType nextComponentType_{0};
      std::vector<std::unique_ptr<ISparseSet>> sparseSets_{};
    };
  }  // namespace ecs

}  // namespace ls
