#pragma once

#include "engine/core/engine_mode.hpp"

namespace ls::event {

  struct EngineModeChanged {
    EngineMode newMode;
  };

}  // namespace ls::event
