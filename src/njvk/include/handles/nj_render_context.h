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
#include <vulkan/vulkan_handles.hpp>

namespace nj::ren {

class Framebuffer : public VulkanObjectNative<vk::Framebuffer> {
public:
    Framebuffer(
        ren::DeviceH device, ren::SwapchainH swapchain,
        ren::RenderPassH renderpass,
        const std::vector<ren::AttachmentDataH>& att
    );

    Framebuffer(
        ren::DeviceH device, ren::SwapchainH swapchain,
        ren::RenderPassH renderpass, std::vector<ren::AttachmentDataH>&& att
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

//! @class RenderContext
//! @brief Handles frame & swapchain image sync and provides
//! tools for constructing and controlling render-loop
//! @details From render-sync side for each frame in flight and/or
//! swapchain image resources might be allocated. Number of
//! per frame/per image resources are different. The reason
//! for this is present-queue-commands sync. Since not a long
//! time ago vk-drivers or smt from just Vulkan side changed
//! and default sync scheme from Vulkan Tutorial does not work
//! anymore.
//!
//! There are 3 different structs per single draw are used:
//! 1. **Command buffer** - are used for recording commands
//!    in it. All commands then will be sobmited to GPU
//!    via using some VkQueue-methods. Submitting for now
//!    happens twice: for graphics (rendering images) and
//!    presenting (showing rendered images at provided
//!    surface)
//!
//! 2. **Sync objects** - Number of sync primitives to make
//!    GPU-GPU-CPU work as effective as it can
//!
//!    - Acquire semaphore - Signals from GPU to GPU
//!      that next image is ready for using
//!
//!    - Submit semaphore - Signals when all submit
//!      work are done
//!
//!    - Frame fence - Sync CPU-render loop side with
//!      GPU. If per frame sync objects are not ready
//!      it will make render loop wait
//!
//! 3. Framebuffers - Handles swapchain stuff ...
//!
class RenderContext {
public:
    RenderContext(
        DeviceH device, SwapchainH swapchain, RenderPassH renderpass,
        CommandPoolH command_pool, size_t frames,
        const std::vector<ren::AttachmentH>& att = {}
    );

    //! @defgroup currents RenderContext Current-methods
    //! There are some current-getters which provides
    //! access to per image or per frame resources
    //! described in \ref nj::ren::RenderContext()
    //! @{

    //! Returns current image index.
    //! @note Values are changed by calling vkAcquireNextImageKHR(...)
    //! inside \ref nj::ren::RenderContext::BeginFrame()
    //! @return Index from range [0..swapchain images count - 1]
    auto CurrentImageIndex() const noexcept -> size_t;

    //! Returns current frame index.
    //! @note Values are changed by the end of each frame render call
    //! inside \ref nj::ren::RenderContext::EndFrame()
    //! @return Index from range [0..frames in flight count - 1]
    auto CurrentFrameIndex() const noexcept -> size_t;

    //! Returns current command buffer.
    //! @note Number of buffer is chosen by \ref ::CurrentFrameIndex()
    //! @return Cmd
    auto CurrentCommandBuffer() noexcept -> CommandBufferH;

    //! Returns current framebufferbuffer.
    //! @note Number of framebuffer is chosen by \ref ::CurrentImageIndex()
    //! @return Framebuffer
    auto CurrentFramebuffer() noexcept -> FramebufferH;

    //! Returns current acquire (available) semaphore.
    //! @note Number of semaphore is chosen by \ref ::CurrentFrameIndex()
    //! @return Acquire semaphore
    auto CurrentAcquireSemaphore() noexcept -> vk::Semaphore;

    //! Returns current submit (finish) semaphore.
    //! @note Number of semaphore is chosen by \ref ::CurrentImageIndex()
    //! @note Using image imdex instead of frame index is needed
    //! to provide correct present submits! It is relevant for
    //! resizing -> changing swapchain extent -> framebuffers
    //! extent -> ...
    //! @return Submit semaphore
    auto CurrentSubmitSemaphore() noexcept -> vk::Semaphore;

    //! Returns current frame fence.
    //! @note Number of fence is chosen by \ref ::CurrentFrameIndex()
    //! @return Frame fence
    auto CurrentFrameFence() noexcept -> vk::Fence;

    //! @} end of currents

    //! Opens frame for recording draw commands.
    //!
    //! @details As this class provides methods for constructing
    //! render loop logic Waits for fence accessed via current
    //! frame index \ref ::RenderContext::CurrentFrameIndex() then
    //! resets ! it and acquires new swapchain image. This move
    //! gives new value for current image index
    //! \ref ::RenderContext::CurrentImageIndex() and begins
    //! command buffer which can be recorded for draw calls and
    //! accessed via \ref ::RenderContext::CurrentCommandBuffer()
    //!
    //! @param device Device to begin frame for
    //! @param swapchain Swappchain to begin frame for
    //! @return Acquiring status
    bool BeginFrame(DeviceH device, SwapchainH swapchain);
    bool EndFrame(
        DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain
    );

    void CreateCmds(DeviceH device, CommandPoolH command_pool);
    void ClearCmds();
    void CreateSyncDatas(ren::DeviceH device);
    void ClearSyncDatas();

    void CreateFramebuffers(
        DeviceH device, SwapchainH swapchain, RenderPassH renderpass,
        const std::vector<AttachmentH>& attachments
    );
    void ClearFramebuffers();

private:
    auto GetNewImage(DeviceH device, SwapchainH swapchain, uint64_t timeout)
        -> bool;

    void ResetFences(DeviceH device);
    void BeginCommandBuffer();
    void EndCommandBuffer();
    void SubmitGraphics(PhysicalDeviceH physical_device);
    bool SubmitPresent(PhysicalDeviceH physical_device, SwapchainH swapchain);
    void WaitFence(DeviceH device, uint64_t timeout);
    void UpdateFrameValue();

private:
    const size_t framesInFlight; //!< Frames running on cpu
    size_t swapchainImages;      //!< Number of swapchain images

    uint32_t currentImageIndex; //!< Swapchain image index for current frame
    uint32_t currentFrameIndex; //!< Frame index for current frame

    std::vector<FramebufferH> framebuffers;
    std::vector<SyncDataH> syncDatas;
    std::vector<CommandBufferH> cmds;
};
using RenderContextH = std::shared_ptr<RenderContext>;

}; // namespace nj::ren

#endif
