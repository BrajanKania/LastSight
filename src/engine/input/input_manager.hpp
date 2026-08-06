#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "engine/input/input_system.hpp"
#include "engine/input/types.hpp"

namespace ls::input {

  class InputManager {
  public:
    void update(bool captureKeyboard, bool captureMouse);

    template <typename TAction>
    void bindKey(Key key) {
      ActionId id{ getActionId<TAction>() };
      ensureCapacity(id);

      bindings_[id] = ActionBinding{
        .type = BindingType::Key,
        .key = key,
      };
    }

    template <typename TAction>
    void bindButton(Button button) {
      ActionId id{ getActionId<TAction>() };
      ensureCapacity(id);

      bindings_[id] = ActionBinding{
        .type = BindingType::Button,
        .button = button,
      };
    }

    template <typename TAction>
    void bindAxis2D(Key up, Key down, Key left, Key right) {
      ActionId id{ getActionId<TAction>() };

      if (id >= axis2D_.size()) {
        axis2D_.resize(id + 1);
        axis2DKeys_.resize(id + 1);
      }

      axis2DKeys_[id] = Axis2DKeys{
        .up = up,
        .down = down,
        .left = left,
        .right = right,
      };
    }

    glm::vec2 getMousePosition() const { return input_system::getMousePosition(); }

    float getScrollDelta() const { return scrollDelta_; }

    template <typename TAction>
    ActionState getActionState() const {
      ActionId id{ getActionId<TAction>() };
      assert(id < actionStates_.size() && "Attempted to get non-mapped action.");
      return actionStates_[id];
    }

    template <typename TAction>
    glm::vec2 getAxis2D() const {
      ActionId id{ getActionId<TAction>() };
      assert(id < axis2D_.size() && "Attempted to get non-mapped axis 2D.");
      return axis2D_[id];
    }

  private:
    template <typename TAction>
    static ActionId getActionId() {
      static const ActionId id{ nextActionId_++ };
      return id;
    }

    void ensureCapacity(ActionId id) {
      if (id >= actionStates_.size()) {
        actionStates_.resize(id + 1);
        bindings_.resize(id + 1);
      }
    }

    float scrollDelta_{ 0.f };

    inline static ActionId nextActionId_{ 0 };
    std::vector<ActionState> actionStates_{};
    std::vector<ActionBinding> bindings_{};

    std::vector<glm::vec2> axis2D_{};
    std::vector<Axis2DKeys> axis2DKeys_{};
  };

}  // namespace ls::input
