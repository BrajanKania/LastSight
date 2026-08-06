#pragma once

namespace ls::component {

  struct Stamina {
    float max{ 100.f };
    float current{ 100.f };
    float regenRate{ 20.f };
    float sprintCostRate{ 10.f };
  };

}  // namespace ls::component
