#include "game/renderer/passes/lit_pass.hpp"

#include <glad/gl.h>

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/texture_2d.hpp"
#include "engine/renderer/command_buffer.hpp"
#include "engine/renderer/render_system.hpp"

namespace ls::renderer {

  void LitPass::onEnter() {
    // clang-format off
    float vertex[] {
      0.f, 1.f,
      0.f, 0.f,
      1.f, 0.f,

      0.f, 1.f,
      1.f, 0.f,
      1.f, 1.f
    };
    // clang-format on

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void LitPass::onExit() {}

  void LitPass::onResize(int width, int height) { litFramebuffer_.resize(width, height); }

  void LitPass::execute(const RenderContext& ctx) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    litFramebuffer_.bind();
    render_system::setClearColor({ 0.2f, 0.2f, 0.2f, 1.f });
    render_system::clearColorBuffer();

    std::vector<const command::DrawSprite*> spritesToDraw;
    spritesToDraw.reserve(ctx.cmdBuffer.getCommands().size());

    for (const auto& cmd : ctx.cmdBuffer.getCommands()) {
      if (auto* spriteCmd{ std::get_if<command::DrawSprite>(&cmd) }) {
        spritesToDraw.push_back(spriteCmd);
      }
    }

    std::stable_sort(spritesToDraw.begin(), spritesToDraw.end(), [](const auto* a, const auto* b) {
      return a->layer < b->layer;
    });

    for (const auto* cmd : spritesToDraw) {
      auto* material{ ctx.materialManager.get(cmd->materialHandle) };
      if (!material)
        continue;

      material->bind();
      material->applyProperties();

      for (const auto& property : cmd->custom) {
        material->applyProperty(property);
      }

      {  // Vertex Shader Uniforms
        material->getShader().setMat4("uModel", cmd->model);
        material->getShader().setMat4("uViewProjection", ctx.frameData.getViewProjection());
        material->getShader().setVec2("uUvScale", cmd->uvScale);
      }

      {  // Fragment Shader Uniforms
        material->getShader().setVec4("uColor", cmd->color);
        material->getShader().setInt("uTexture", 0);
      }

      if (cmd->textureHandle.isValid()) {
        if (auto* texture{ ctx.textureManager.get(cmd->textureHandle) }) {
          texture->bind(0);
        }
      }

      render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);
      material->resetTemporaryProperties();
    }

    litFramebuffer_.unbind();
    ctx.frameData.addFramebuffer("lit", &litFramebuffer_);
  }

}  // namespace ls::renderer
