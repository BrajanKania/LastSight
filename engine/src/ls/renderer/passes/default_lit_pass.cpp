#include "ls/renderer/passes/default_lit_pass.hpp"

#include <glad/gl.h>

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

#include "ls/gfx/framebuffer.hpp"
#include "ls/gfx/texture_2d.hpp"
#include "ls/renderer/command_buffer.hpp"
#include "ls/renderer/render_system.hpp"

namespace ls::renderer {

  void DefaultLitPass::onEnter() {
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

  void DefaultLitPass::onExit() {}

  void DefaultLitPass::onResize(int width, int height) { litFBO_.resize(width, height); }

  void DefaultLitPass::execute(const RenderContext& ctx) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    litFBO_.bind();
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

    glBindVertexArray(vao_);

    for (const auto* cmd : spritesToDraw) {
      auto* material{ ctx.materialManager.get(cmd->materialHandle) };
      if (!material)
        continue;

      material->bind();
      material->applyProperties();

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
    }

    glBindVertexArray(0);

    litFBO_.unbind();
    ctx.frameData.addFramebuffer("lit", &litFBO_);
  }

}  // namespace ls::renderer
