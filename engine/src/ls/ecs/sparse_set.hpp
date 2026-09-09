#pragma once

#include <cassert>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/meta/meta.hpp>
#include <vector>

#include "ls/ecs/types.hpp"

namespace ls::ecs {

  class ISparseSet {
  public:
    virtual ~ISparseSet() = default;
    virtual void addComponent(EntityId entity) = 0;
    virtual void destroyComponent(EntityId entity) = 0;
    virtual void copyComponent(EntityId sourceEntity, EntityId targetEntity) = 0;
    virtual bool hasComponent(EntityId entity) const = 0;
    virtual entt::id_type getTypeId() const = 0;
    virtual void* getRawComponent(EntityId entity) = 0;
    virtual bool emplaceMeta(EntityId entity, const entt::meta_any& componentAny) = 0;
    virtual void clear() = 0;
  };

  template <typename TComponent>
  class SparseSet : public ISparseSet {
  public:
    void addComponent(EntityId entity) override { addComponent(entity, TComponent{}); }

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
      if (hasComponent(entity))
        return;

      if (sparse_.size() <= entity) {
        sparse_.resize(entity + 1, kNullEntity);
      }

      sparse_[entity] = dense_.size();
      dense_.push_back(entity);
      components_.push_back(std::move(component));
    }

    void copyComponent(EntityId sourceEntity, EntityId targetEntity) override {
      if (hasComponent(sourceEntity)) {
        addComponent(targetEntity, getComponent(sourceEntity));
      }
    }

    bool hasComponent(EntityId entity) const override {
      return (entity < sparse_.size() && sparse_[entity] != kNullEntity);
    }

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

    bool emplaceMeta(EntityId entity, const entt::meta_any& componentAny) override {
      if (hasComponent(entity)) {
        destroyComponent(entity);
      }

      if (const auto* ptr{ componentAny.try_cast<TComponent>() }) {
        addComponent(entity, *ptr);
        return true;
      }

      if (auto copy{ componentAny.allow_cast<TComponent>() }) {
        addComponent(entity, copy.template cast<TComponent>());
        return true;
      }

      return false;
    }

    void clear() override {
      sparse_.clear();
      dense_.clear();
      components_.clear();
    }

  private:
    std::vector<EntityId> sparse_{};
    std::vector<EntityId> dense_{};
    std::vector<TComponent> components_{};
  };

}  // namespace ls::ecs
