#pragma once

#include <cassert>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <vector>

#include "engine/ecs/types.hpp"

namespace ls::ecs {

  class ISparseSet {
  public:
    virtual ~ISparseSet() = default;
    virtual void destroyComponent(EntityId entity) = 0;
    virtual entt::id_type getTypeId() const = 0;
    virtual void* getRawComponent(EntityId entity) = 0;
  };

  template <typename TComponent>
  class SparseSet : public ISparseSet {
  public:
    void destroyComponent(EntityId entity) override {
      if (!hasComponent(entity))
        return;

      EntityId indexOfRemoved{ sparse_[entity] };
      std::size_t lastIndex{ dense_.size() - 1 };

      if (indexOfRemoved != lastIndex) {
        EntityId entityOfLastElement{ dense_.back() };

        dense_[indexOfRemoved] = entityOfLastElement;
        components_[indexOfRemoved] = std::move(components_.back());
        sparse_[entityOfLastElement] = indexOfRemoved;
      }

      components_.pop_back();
      dense_.pop_back();
      sparse_[entity] = kNullEntity;
    }

    void addComponent(EntityId entity, TComponent component) {
      if (sparse_.size() <= entity) {
        sparse_.resize(entity + 1, kNullEntity);
      }

      sparse_[entity] = dense_.size();
      dense_.push_back(entity);
      components_.push_back(std::move(component));
    }

    bool hasComponent(EntityId entity) const { return (entity < sparse_.size() && sparse_[entity] != kNullEntity); }

    TComponent& getComponent(EntityId entity) {
      assert(hasComponent(entity) && "Attempted to get component that entity does not have.");
      return components_[sparse_[entity]];
    }

    const TComponent& getComponent(EntityId entity) const {
      assert(hasComponent(entity) && "Attempted to get component that entity does not have.");
      return components_[sparse_[entity]];
    }

    const std::vector<EntityId>& getEntities() const { return dense_; }

    entt::id_type getTypeId() const override { return entt::type_id<TComponent>().hash(); }

    void* getRawComponent(EntityId entity) override {
      return hasComponent(entity) ? &components_[sparse_[entity]] : nullptr;
    }

  private:
    std::vector<EntityId> sparse_{};
    std::vector<EntityId> dense_{};
    std::vector<TComponent> components_{};
  };

}  // namespace ls::ecs
