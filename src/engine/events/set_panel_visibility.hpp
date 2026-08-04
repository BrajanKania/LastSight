#pragma once

#include <string>
namespace ls::event {

  struct SetPanelVisibility {
    std::string name;
    bool visible{ false };
  };

}  // namespace ls::event
