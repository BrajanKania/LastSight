#pragma once

#include <variant>

#include "ls/renderer/commands/draw_sprite.hpp"

namespace ls::renderer {

  using RenderCommand = std::variant<command::DrawSprite>;

}
