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
    Framebuffer(
        ren::DeviceH device,
        ren::SwapchainH swapchain,
        ren::RenderPassH renderpass,
        const std::vector<ren::AttachmentDataH>& att
    );

    Framebuffer(
        ren::DeviceH device,
        ren::SwapchainH swapchain,
        ren::RenderPassH renderpass,
        std::vector<ren::AttachmentDataH>&& att
    );

    auto Attachement(size_t idx) -> const ren::AttachmentDataH&;
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
                  size_t frames, size_t frame_objects_mode, 
                  const std::vector<ren::AttachmentH> &att = {});

    auto CurrentImageIndex() const noexcept -> size_t;
    auto CurrentFrameIndex() const noexcept -> size_t;
    auto CurrentCommandBuffer() noexcept -> CommandBufferH;
    auto CurrentFramebuffer() noexcept -> FramebufferH;
    auto CurrentSyncData() noexcept -> SyncDataH;

    bool BeginFrame(DeviceH device, SwapchainH swapchain);
    bool EndFrame(DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain);

    void CreateCmds(DeviceH device, CommandPoolH command_pool);
    void ClearCmds();
    void CreateSyncDatas(ren::DeviceH device);
    void ClearSyncDatas();

    void CreateFramebuffers(DeviceH device, SwapchainH swapchain, RenderPassH renderpass,
                             const std::vector<AttachmentH> &attachments);
    void ClearFramebuffers();
 
    
  private:
    auto GetNewImage(DeviceH device, SwapchainH swapchain, uint64_t timeout)-> bool;
    auto GetNewImageForPerFrame(DeviceH device, SwapchainH swapchain, uint64_t timeout) -> bool;
    auto GetNewImageForPerImage(DeviceH device, SwapchainH swapchain, uint64_t timeout) -> bool;

    void ResetFences(DeviceH device);
    void BeginCommandBuffer();
    void EndCommandBuffer();
    void SubmitGraphics(PhysicalDeviceH physical_device);
    bool SubmitPresent(PhysicalDeviceH physical_device, SwapchainH swapchain);
    void WaitFence(DeviceH device, uint64_t timeout);
    void UpdateFrameValue();

  private:
    const size_t frameObjectsMode;
    const size_t framesInFlight;
    size_t swapchainImages;

    uint32_t currentImageIndex;
    uint32_t currentFrameIndex;
  
    std::vector<FramebufferH> framebuffers;
    std::vector<SyncDataH> syncDatas;
    std::vector<CommandBufferH> cmds;
};
using RenderContextH = std::shared_ptr<RenderContext>;

// clang-format on

}; // namespace nj::ren

#endif
