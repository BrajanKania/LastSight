#pragma once

#include <algorithm>
#include <cassert>
#include <memory>

#include "engine/ecs/sparse_set.hpp"
#include "engine/ecs/types.hpp"
#include "engine/ecs/view.hpp"

namespace ls::ecs {

  class Registry {
  public:
    EntityId createEntity() {
      EntityId entity;

      if (!availableEntities_.empty()) {
        entity = availableEntities_.back();
        availableEntities_.pop_back();
      } else {
        entity = nextEntity_++;
      }

      return entity;
    }

    void destroyEntity(EntityId entity) {
      if (isEntityPendingDestroy(entity))
        return;

      entitiesToDestroy_.push_back(entity);
    }

    void purgeDestroyedEntities() {
      for (auto& sparseSet : sparseSets_) {
        if (sparseSet) {
          for (auto entity : entitiesToDestroy_) {
            sparseSet->destroyComponent(entity);
          }
        }
      }

      for (auto entity : entitiesToDestroy_) {
        availableEntities_.push_back(entity);
      }

      entitiesToDestroy_.clear();
    }

    bool isValidEntity(EntityId entity) const {
      if (entity >= nextEntity_)
        return false;

      return std::ranges::find(availableEntities_, entity) == availableEntities_.end();
    }

    bool isEntityPendingDestroy(EntityId entity) const {
      return std::ranges::find(entitiesToDestroy_, entity) != entitiesToDestroy_.end();
    }

    template <typename TComponent>
    bool hasComponent(EntityId entity) const {
      if (!isValidEntity(entity))
        return false;

      const auto* sparseSet{ getSparseSetPointer<TComponent>() };
      if (!sparseSet)
        return false;

      return sparseSet->hasComponent(entity);
    }

    template <typename TComponent>
    void addComponent(EntityId entity, TComponent component) {
      assert(isValidEntity(entity) && "Attempted to add component to an invalid entity.");
      assert(!hasComponent<TComponent>(entity) && "Entity already has this component.");

      getSparseSetPointer<TComponent>()->addComponent(entity, std::move(component));
    }

    template <typename TComponent>
    void destroyComponent(EntityId entity) {
      assert(isValidEntity(entity) && "Attempted to destroy component on an invalid entity.");
      assert(hasComponent<TComponent>(entity) && "Entity does not have the component to destroy.");

      getSparseSetPointer<TComponent>()->destroyComponent(entity);
    }

    template <typename TComponent>
    TComponent& getComponent(EntityId entity) {
      assert(isValidEntity(entity) && "Attempted to get component from an invalid entity.");

      auto* sparseSet{ getSparseSetPointer<TComponent>() };
      assert(sparseSet->hasComponent(entity) && "Entity does not have the requested component.");

      return sparseSet->get(entity);
    }

    template <typename TComponent>
    const TComponent& getComponent(EntityId entity) const {
      assert(isValidEntity(entity) && "Attempted to get component from an invalid entity.");

      const auto* sparseSet{ getSparseSetPointer<TComponent>() };
      assert(sparseSet && "Attempted to get unregistered component type.");
      assert(sparseSet->hasComponent(entity) && "Entity does not have the requested component.");

      return sparseSet->get(entity);
    }

    template <typename... TComponents>
    auto view() const {
      return View<TComponents...>(getSparseSetPointer<TComponents>()...);
    }

  private:
    template <typename TComponent>
    static ComponentId getComponentId() {
      static ComponentId id{ nextComponentId_++ };
      return id;
    }

    template <typename TComponent>
    void registerComponent() {
      ComponentId componentId{ getComponentId<TComponent>() };

      if (sparseSets_.size() <= componentId) {
        sparseSets_.resize(componentId + 1);
      }

      sparseSets_[componentId] = std::make_unique<SparseSet<TComponent>>();
    }

    template <typename TComponent>
    SparseSet<TComponent>* getSparseSetPointer() {
      ComponentId componentId{ getComponentId<TComponent>() };

      if (sparseSets_.size() <= componentId || !sparseSets_[componentId]) {
        registerComponent<TComponent>();
      }

      return static_cast<SparseSet<TComponent>*>(sparseSets_[componentId].get());
    }

    template <typename TComponent>
    const SparseSet<TComponent>* getSparseSetPointer() const {
      ComponentId componentId{ getComponentId<TComponent>() };

      if (sparseSets_.size() <= componentId) {
        return nullptr;
      }

      return static_cast<const SparseSet<TComponent>*>(sparseSets_[componentId].get());
    }

    EntityId nextEntity_{ 0 };
    std::vector<EntityId> availableEntities_{};

    inline static ComponentId nextComponentId_{ 0 };
    std::vector<std::unique_ptr<ISparseSet>> sparseSets_{};
    std::vector<EntityId> entitiesToDestroy_{};
  };

}  // namespace ls::ecs
