#pragma once

#include <cstdint>

namespace ls::layer {

  enum : int32_t {
    Background = -100,
    Ground = 0,
    Entities = 10,
    Foreground = 100,
  };

}
