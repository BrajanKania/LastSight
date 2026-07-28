#pragma once

#include <memory>

#include "engine/ecs/sparse_set.hpp"
#include "engine/ecs/types.hpp"
#include "engine/ecs/view.hpp"

namespace ls::ecs {

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

}  // namespace ls::ecs
