#pragma once
#ifndef NJ_RENDER_CONTEXT_H
#define NJ_RENDER_CONTEXT_H

#include "nj_attachment.h"
#include "nj_command_buffer.h"
#include "nj_device.h"
#include "nj_render_pass.h"
#include "nj_render_sync.h"
#include "nj_swapchain.h"
#include <memory>
#include <vector>

namespace nj::ren {

class Framebuffer : public VulkanObject<vk::Framebuffer> {
  public:
    Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                ren::RenderPassH renderpass,
                const std::vector<ren::AttachmentDataH> &att);

    Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                ren::RenderPassH renderpass,
                std::vector<ren::AttachmentDataH> &&att);

    auto Attachement(size_t idx) -> const ren::AttachmentDataH &;
    auto HandleName() const noexcept -> std::string override;

  private:
    std::vector<ren::AttachmentDataH> attachments;
};
using FramebufferH = std::shared_ptr<Framebuffer>;
//
//
//
//
//
class FrameContext {
  public:
    FrameContext(FramebufferH, CommandBufferH, SyncDataH);
    FrameContext(FrameContext &&) noexcept = default;

    FramebufferH framebuffer;
    CommandBufferH commandBuffer;
    SyncDataH syncData;
};
using FrameContextH = std::shared_ptr<FrameContext>;
//
//
//
//
//
class RenderContext {
  public:
    RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                  ren::RenderPassH renderpass, ren::CommandPoolH command_pool,
                  size_t frames, const std::vector<ren::AttachmentH> &att = {});

    void CleanUp();

    auto Context(size_t frame) -> FrameContextH &;

  private:
    void CreateFrameContexts(ren::DeviceH device, ren::SwapchainH swapchain,
                             ren::RenderPassH renderpass,
                             ren::CommandPoolH command_pool, size_t frames,
                             const std::vector<ren::AttachmentH> &att);

  private:
    std::vector<FrameContextH> contexts;
};
using RenderContextH = std::shared_ptr<RenderContext>;

}; // namespace nj::ren

#endif
