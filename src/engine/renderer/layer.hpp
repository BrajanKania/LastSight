#pragma once

#include <cstdint>

namespace ls::renderer {

  enum Layer : int32_t {
    Background = -100,
    Ground = 0,
    Entities = 10,
    Foreground = 100,
  };

}
