#pragma once
#include "nj_physical_device.h"
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
class ImageContext {
  public:
    ImageContext(FramebufferH framebuffer);
    FramebufferH framebuffer;
};
using ImageContextH = std::shared_ptr<ImageContext>;
//
//
//
class FrameContext {
  public:
    FrameContext(CommandBufferH, SyncDataH);
    FrameContext(FrameContext &&) noexcept = default;

    CommandBufferH commandBuffer;
    SyncDataH syncData;
};
using FrameContextH = std::shared_ptr<FrameContext>;
//
//
//
//
// clang-format off
class RenderContext {
  public:
    RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                  ren::RenderPassH renderpass, ren::CommandPoolH command_pool,
                  size_t frames, const std::vector<ren::AttachmentH> &att = {});

    void CleanUp();

    auto GetFrameContext(size_t frame) -> FrameContextH ;
    auto GetImageContext(size_t image) -> ImageContextH ;

    auto CurrentImageIndex() const noexcept -> size_t;
    auto CurrentFrameIndex() const noexcept -> size_t;
    auto CurrentCommandBuffer() noexcept -> CommandBufferH;
    auto CurrentFramebuffer() noexcept -> FramebufferH;

    void BeginFrame(DeviceH device, SwapchainH swapchain);
    void EndFrame(DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain);

  private:
    void CreateFrameContexts(DeviceH device, CommandPoolH command_pool, size_t frames);
    void CreateImageContexts(DeviceH device, SwapchainH swapchain, RenderPassH renderpass,
                             const std::vector<AttachmentH> &attachments);
    auto GetNewImage(DeviceH device, SwapchainH swapchain, FrameContextH frame_ctx, 
                     uint64_t timeout)-> size_t;

    void ResetFences(DeviceH device, FrameContextH frame_ctx);
    void BeginCommandBuffer(FrameContextH frame_ctx);
    void EndCommandBuffer(FrameContextH frame_ctx);
    void SubmitGraphics(FrameContextH frame_ctx, PhysicalDeviceH physical_device);
    void SubmitPresent(FrameContextH frame_ctx, PhysicalDeviceH physical_device, 
                       SwapchainH swapchain);
    void WaitFences(DeviceH device, FrameContextH frame_ctx, uint64_t timeout);

  private:
    const size_t framesInFlight;
    size_t currentImageIndex;
    size_t currentFrameIndex;
  
    std::vector<FrameContextH> frameContexts;
    std::vector<ImageContextH> imageContexts;
};
using RenderContextH = std::shared_ptr<RenderContext>;

// clang-format on

}; // namespace nj::ren

#endif
