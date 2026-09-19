#pragma once

#include "ls/asset/asset_context.hpp"
#include "ls/core/engine_context.hpp"
#include "ls/prefab/prefab_context.hpp"

namespace ls {

  namespace debug {
    class Console;
  }
  namespace prefab {
    class PrefabManager;
  }
  namespace renderer {
    class MaterialManager;
  }

}  // namespace ls

namespace ls::serialization {

  struct SerializationContext {
    debug::Console* console{ nullptr };
    prefab::PrefabManager* prefabManager{ nullptr };
    renderer::MaterialManager* materialManager{ nullptr };

    SerializationContext() = default;

    SerializationContext(EngineContext ctx)
        : console{ ctx.console },
          prefabManager{ ctx.prefabManager },
          materialManager{ ctx.materialManager } {}

    SerializationContext(prefab::PrefabContext ctx)
        : console{ ctx.console },
          materialManager{ ctx.materialManager },
          prefabManager{ ctx.prefabManager } {}

    SerializationContext(asset::AssetContext ctx)
        : console{ ctx.console } {}
  };

}  // namespace ls::serialization
