#include "engine/input/input_manager.hpp"

#include <algorithm>
#include <bit>
#include <glm/ext/quaternion_geometric.hpp>
#include <numeric>

#include "engine/input/input_context.hpp"
#include "engine/input/input_system.hpp"
#include "engine/input/types.hpp"

namespace ls::input {

  static Modifier getModifierBit(Key key) {
    if (key == Key::LCtrl || key == Key::RCtrl)
      return static_cast<Modifier>(KeyModifier::Ctrl);
    if (key == Key::LShift || key == Key::RShift)
      return static_cast<Modifier>(KeyModifier::Shift);
    if (key == Key::LAlt || key == Key::RAlt)
      return static_cast<Modifier>(KeyModifier::Alt);
    return 0;
  }

  void InputManager::update(InputContext& inputCtx) {
    scrollDelta_ = inputCtx.captureMouse ? 0.f : input_system::getScrollDelta();

    Modifier currentModifiers{ 0 };
    if (input_system::isKeyPressed(Key::LCtrl) || input_system::isKeyPressed(Key::RCtrl)) {
      currentModifiers |= KeyModifier::Ctrl;
    }
    if (input_system::isKeyPressed(Key::LShift) || input_system::isKeyPressed(Key::RShift)) {
      currentModifiers |= KeyModifier::Shift;
    }
    if (input_system::isKeyPressed(Key::LAlt) || input_system::isKeyPressed(Key::RAlt)) {
      currentModifiers |= KeyModifier::Alt;
    }

    std::vector<ActionId> sortedIds(bindings_.size());
    std::iota(sortedIds.begin(), sortedIds.end(), 0);

    std::stable_sort(sortedIds.begin(), sortedIds.end(), [this](ActionId a, ActionId b) {
      return std::popcount(bindings_[a].modifiers) > std::popcount(bindings_[b].modifiers);
    });

    for (ActionId id : sortedIds) {
      const auto& binding{ bindings_[id] };
      bool isDown{ false };

      if (binding.type == BindingType::Key && !inputCtx.captureKeyboard) {
        const Modifier keyModifierBit{ getModifierBit(binding.key) };
        const Modifier effectiveModifiers{ static_cast<Modifier>(currentModifiers & ~keyModifierBit) };

        bool modifiersMatch{ false };
        if (binding.modifiers > 0) {
          modifiersMatch = (binding.modifiers == effectiveModifiers);
        } else {
          modifiersMatch = true;
        }

        const bool isKeyPressed{ input_system::isKeyPressed(binding.key) };
        const bool isKeyConsumed{ inputCtx.consumedInputState.keys.contains(binding.key) };

        if (modifiersMatch && isKeyPressed && !isKeyConsumed) {
          isDown = true;
          inputCtx.consumedInputState.keys.insert(binding.key);
        }
      } else if (binding.type == BindingType::Button && !inputCtx.captureMouse) {
        const bool isButtonPressed{ input_system::isButtonPressed(binding.button) };
        const bool isButtonConsumed{ inputCtx.consumedInputState.buttons.contains(binding.button) };

        if (isButtonPressed && !isButtonConsumed) {
          isDown = true;
          inputCtx.consumedInputState.buttons.insert(binding.button);
        }
      }

      ActionState& state{ actionStates_[id] };
      const bool wasDown{ (state == ActionState::JustPressed || state == ActionState::Held) };

      if (isDown) {
        state = wasDown ? ActionState::Held : ActionState::JustPressed;
      } else {
        state = wasDown ? ActionState::JustReleased : ActionState::Up;
      }
    }

    for (ActionId id{ 0 }; id < axis2DKeys_.size(); id++) {
      if (inputCtx.captureKeyboard) {
        axis2D_[id] = glm::vec2(0.f);
        continue;
      }

      const auto& keys{ axis2DKeys_[id] };
      glm::vec2 dir{ 0.f };

      auto isAxisKeyActive{ [&](Key key) {
        return !inputCtx.consumedInputState.keys.contains(key) && input_system::isKeyPressed(key);
      } };

      if (isAxisKeyActive(keys.up))
        dir.y += 1.f;
      if (isAxisKeyActive(keys.down))
        dir.y -= 1.f;
      if (isAxisKeyActive(keys.left))
        dir.x -= 1.f;
      if (isAxisKeyActive(keys.right))
        dir.x += 1.f;

      float length{ glm::length(dir) };
      axis2D_[id] = (length != 0.f) ? glm::normalize(dir) : glm::vec2(0.f);
    }
  }

}  // namespace ls::input
