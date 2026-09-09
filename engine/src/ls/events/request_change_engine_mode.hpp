#pragma once

#include "ls/core/engine_mode.hpp"

namespace ls::event {

  struct RequestChangeEngineMode {
    EngineMode newMode;
  };

}  // namespace ls::event
