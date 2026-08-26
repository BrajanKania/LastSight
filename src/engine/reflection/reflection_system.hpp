#pragma once

namespace ls::reflection_system {

  struct ComponentInfo {
    const char* displayName{ nullptr };
    bool isComponent{ true };
  };

  struct PropertyInfo {
    const char* displayName{ nullptr };
    bool readOnly{ false };
    bool transient{ false };
    bool color{ false };
    float step{ 0.0f };
    bool range{ false };
    float rangeMin{ 0.f };
    float rangeMax{ 1.f };
  };

  struct EnumInfo {
    const char* displayName{ nullptr };
  };

  void registerGeneratedTypes();

}  // namespace ls::reflection_system
