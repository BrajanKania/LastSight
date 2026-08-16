#pragma once

#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"

namespace ls::serialization {

  class SceneSerializer {
  public:
    explicit SceneSerializer(SceneContext sceneCtx, EngineContext engineCtx)
        : sceneCtx_{ sceneCtx },
          engineCtx_{ engineCtx } {}

    bool loadScene(const std::filesystem::path& path);
    bool saveScene(const std::filesystem::path& path);

  private:
    SceneContext sceneCtx_;
    EngineContext engineCtx_;
  };

}  // namespace ls::serialization
