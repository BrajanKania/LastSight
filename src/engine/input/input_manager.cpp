#include "engine/input/input_manager.hpp"

#include <glm/ext/quaternion_geometric.hpp>

#include "engine/input/input_system.hpp"
#include "engine/input/types.hpp"

namespace ls::input {

  void InputManager::update(bool captureKeyboard, bool captureMouse) {
    float scrollDelta{ input_system::getScrollDelta() };
    scrollDelta_ = captureMouse ? 0.f : scrollDelta;

    for (ActionId id{ 0 }; id < bindings_.size(); id++) {
      const auto& binding{ bindings_[id] };
      bool isDown{ false };

      if (binding.type == BindingType::Key && !captureKeyboard) {
        isDown = input_system::isKeyPressed(binding.key);
      } else if (binding.type == BindingType::Button && !captureMouse) {
        isDown = input_system::isButtonPressed(binding.button);
      }

      ActionState& state{ actionStates_[id] };
      bool wasDown{ (state == ActionState::JustPressed || state == ActionState::Held) };

      if (isDown) {
        state = wasDown ? ActionState::Held : ActionState::JustPressed;
      } else {
        state = wasDown ? ActionState::JustReleased : ActionState::Up;
      }
    }

    for (ActionId id{ 0 }; id < axis2DKeys_.size(); id++) {
      if (captureKeyboard) {
        axis2D_[id] = glm::vec2(0.f);
        continue;
      }

      const auto& keys{ axis2DKeys_[id] };
      glm::vec2 dir{ 0.f };

      if (input_system::isKeyPressed(keys.up))
        dir.y += 1.f;
      if (input_system::isKeyPressed(keys.down))
        dir.y -= 1.f;
      if (input_system::isKeyPressed(keys.left))
        dir.x -= 1.f;
      if (input_system::isKeyPressed(keys.right))
        dir.x += 1.f;

      float length{ glm::length(dir) };
      axis2D_[id] = (length != 0.f) ? glm::normalize(dir) : glm::vec2(0.f);
    }
  }

}  // namespace ls::input
