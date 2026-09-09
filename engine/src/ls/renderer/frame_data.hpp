#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <unordered_map>

#include "ls/gfx/framebuffer.hpp"
#include "ls/gfx/texture_2d.hpp"

namespace ls::renderer {

  class FrameData {
  public:
    void addFramebuffer(const std::string& name, gfx::Framebuffer* framebuffer) {
      nameToFramebuffer_[name] = framebuffer;
    }

    gfx::Framebuffer* getFramebuffer(const std::string& name) {
      auto it{ nameToFramebuffer_.find(name) };

      if (it != nameToFramebuffer_.end())
        return it->second;

      return nullptr;
    }

    const gfx::Framebuffer* getFramebuffer(const std::string& name) const {
      auto it{ nameToFramebuffer_.find(name) };

      if (it != nameToFramebuffer_.end())
        return it->second;

      return nullptr;
    }

    const std::unordered_map<std::string, gfx::Framebuffer*>& getFramebuffers() const { return nameToFramebuffer_; }

    const gfx::Texture2D* getColorTexture(const std::string& name) const {
      auto* framebuffer{ getFramebuffer(name) };

      if (framebuffer)
        return &framebuffer->getColorTexture();

      return nullptr;
    }

    void clear() { nameToFramebuffer_.clear(); }

    void setCamera(glm::mat4 view, glm::mat4 projection) {
      view_ = view;
      projection_ = projection;
    }

    const glm::mat4& getView() const { return view_; }

    const glm::mat4& getProjection() const { return projection_; }

    glm::mat4 getViewProjection() const { return projection_ * view_; }

  private:
    glm::mat4 view_{ 1.f };
    glm::mat4 projection_{ 1.f };
    std::unordered_map<std::string, gfx::Framebuffer*> nameToFramebuffer_{};
  };

}  // namespace ls::renderer
