#pragma once

#include <optional>

#include "core/ecs.hpp"
#include "core/scene.hpp"
#include "renderer/shader.hpp"

namespace ls {

  class WorldScene : public IScene {
  public:
    void onEnter() override;
    void onExit() override;

    void handleInput() override;
    void update(float dt) override;
    void render() override;

    bool isOpaque() const override { return true; }

  private:
    ecs::Registry registry_;
    std::optional<Shader> shader_;
    unsigned int vao_;
    unsigned int vbo_;
  };

}  // namespace ls
