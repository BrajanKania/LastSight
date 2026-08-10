#pragma once

namespace ls::reflection {

  struct PropertyInfo {
    const char* displayName{ nullptr };
    bool readOnly{ false };
  };

  void registerGeneratedTypes();

}  // namespace ls::reflection
