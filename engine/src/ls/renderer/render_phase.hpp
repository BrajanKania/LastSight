#pragma once

#include <cstdint>

namespace ls::renderer {

  enum class RenderPhase : std::uint8_t {
    PrePass = 0,
    MainPass,
    PostProcessPass,
    UIPass,
    DebugPass,
  };

}
