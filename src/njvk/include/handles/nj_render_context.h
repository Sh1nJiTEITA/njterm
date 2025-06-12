#pragma once
#ifndef NJ_RENDER_CONTEXT_H
#define NJ_RENDER_CONTEXT_H

#include <memory>
#include <nj_builder.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

// class BUILDABLE(FrameData) {
class FrameData {
  public: // FRIENDS <3 -------------------
    friend build::Builder<FrameData>;

  public:
    FrameData() = default;

    auto Image() -> vk::SharedImage &;
    auto View() -> vk::SharedImageView &;
    auto Framebuffer() -> vk::SharedFramebuffer &;
    auto Attachments() -> std::vector<vk::SharedImageView> &;

  private:
    //! Swapchain image pointer ( creates from swapchain )
    vk::SharedImage image;
    //! View for swapchain image to use it in renderpass
    vk::SharedImageView view;
    vk::SharedFramebuffer framebuffer;
    //! Attachements for current framebuffer ( using inside renderpass )
    std::vector<vk::SharedImageView> attachments;
};
using FrameDataH = std::shared_ptr<FrameData>;

class RenderContext {
  public: // FRIENDS <3 -------------------
    friend build::Builder<RenderContext>;

  public:
    RenderContext() = default;

  protected:
  private:
    std::vector<FrameDataH> frameDatas;
};
using RenderContextH = std::shared_ptr<RenderContext>;

}; // namespace nj::ren

#endif
