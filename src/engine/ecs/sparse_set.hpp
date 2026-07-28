#pragma once

#include <vector>

#include "engine/ecs/types.hpp"

namespace ls::ecs {

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

}  // namespace ls::ecs
