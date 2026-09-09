#pragma once

#include <string>

namespace ls::event {

  struct RequestCreateEntity {
    std::string entityName{ "new_entity" };
  };

}  // namespace ls::event
