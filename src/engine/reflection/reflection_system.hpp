#pragma once

namespace ls::reflection_system {

  struct PropertyInfo {
    const char* displayName{ nullptr };
    bool readOnly{ false };
    bool transient{ false };
  };

  void registerGeneratedTypes();

}  // namespace ls::reflection_system
