#pragma once
#ifndef NJ_RENDER_CONTEXT_H
#define NJ_RENDER_CONTEXT_H

#include "nj_attachment.h"
#include "nj_command_buffer.h"
#include "nj_device.h"
#include "nj_physical_device.h"
#include "nj_render_pass.h"
#include "nj_render_sync.h"
#include "nj_swapchain.h"
#include <memory>
#include <vector>

namespace nj::ren {

class Framebuffer : public VulkanObjectNative<vk::Framebuffer> {
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
    RenderContext(DeviceH device, SwapchainH swapchain,
                  RenderPassH renderpass, CommandPoolH command_pool,
                  size_t frames, const std::vector<ren::AttachmentH> &att = {});

    void CleanUp();

    auto GetFrameContext(size_t frame) -> FrameContextH ;
    auto GetImageContext(size_t image) -> ImageContextH ;

    auto CurrentImageIndex() const noexcept -> size_t;
    auto CurrentFrameIndex() const noexcept -> size_t;
    auto CurrentCommandBuffer() noexcept -> CommandBufferH;
    auto CurrentFramebuffer() noexcept -> FramebufferH;

    bool BeginFrame(DeviceH device, SwapchainH swapchain);
    bool EndFrame(DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain);

    void CreateFrameContexts(DeviceH device, CommandPoolH command_pool, size_t frames);
    void ClearFrameContexts();
    void CreateImageContexts(DeviceH device, SwapchainH swapchain, RenderPassH renderpass,
                             const std::vector<AttachmentH> &attachments);
    void ClearImageContexts();
 
    void WaitFences(DeviceH device, SwapchainH swapchain);
    
  private:
    auto GetNewImage(DeviceH device, SwapchainH swapchain, FrameContextH frame_ctx, uint64_t timeout)-> bool;

    void ResetFences(DeviceH device, FrameContextH frame_ctx);
    void BeginCommandBuffer(FrameContextH frame_ctx);
    void EndCommandBuffer(FrameContextH frame_ctx);
    void SubmitGraphics(FrameContextH frame_ctx, PhysicalDeviceH physical_device);
    bool SubmitPresent(FrameContextH frame_ctx, PhysicalDeviceH physical_device, 
                       SwapchainH swapchain);
    void WaitFence(DeviceH device, FrameContextH frame_ctx, uint64_t timeout);

  private:
    const size_t framesInFlight;
    uint32_t currentImageIndex;
    uint32_t currentFrameIndex;
  
    std::vector<FrameContextH> frameContexts;
    std::vector<ImageContextH> imageContexts;
};
using RenderContextH = std::shared_ptr<RenderContext>;

// clang-format on

}; // namespace nj::ren

#endif
