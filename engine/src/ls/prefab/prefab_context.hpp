#pragma once

namespace ls {

  namespace debug {
    class Console;
  }
  namespace renderer {
    class MaterialManager;
  }

  namespace prefab {
    class PrefabManager;

    struct PrefabContext {
      PrefabManager* prefabManager{ nullptr };
      renderer::MaterialManager* materialManager{ nullptr };
      debug::Console* console{ nullptr };
    };

  }  // namespace prefab

}  // namespace ls
