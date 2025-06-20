#pragma once
#ifndef NJ_RENDER_CONTEXT_H
#define NJ_RENDER_CONTEXT_H

#include "nj_attachment.h"
#include "nj_device.h"
#include "nj_render_pass.h"
#include "nj_swapchain.h"
#include <memory>
#include <vector>

namespace nj::ren {

class Framebuffer : VulkanObject<vk::Framebuffer> {
  public:
    Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                ren::RenderPassH renderpass,
                const std::vector<ren::AttachmentDataH> &att);

    auto Attachement(size_t idx) -> const ren::AttachmentDataH &;
    auto HandleName() const noexcept -> std::string override;

  private:
    std::vector<ren::AttachmentDataH> attachments;
};
using FramebufferH = std::shared_ptr<Framebuffer>;

class RenderContext {
  public:
    RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                  ren::RenderPassH renderpass,
                  const std::vector<ren::AttachmentH> &att);

  private:
    std::vector<FramebufferH> framebuffers;
};
using RenderContextH = std::shared_ptr<RenderContext>;

}; // namespace nj::ren

#endif
