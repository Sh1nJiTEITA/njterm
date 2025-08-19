#include "nj_render_context.h"
#include "nj_command_buffer.h"
#include "nj_device.h"
#include "nj_render_pass.h"
#include "nj_swapchain.h"
#include "njcon.h"
#include "njlog.h"
#include <limits>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

auto Framebuffer::Attachement(size_t idx) -> const ren::AttachmentDataH & {
    return attachments.at(idx);
}

Framebuffer::Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                         ren::RenderPassH renderpass,
                         const std::vector<ren::AttachmentDataH> &att)
    : attachments{att} {
    if (attachments.empty()) {
        log::Warn("Empty attachments array inside framebuffer ctor");
        return;
    }
    std::vector<vk::ImageView> views;
    views.reserve(att.size());
    for (const auto &a : attachments) {
        views.push_back(*a->imageView);
    }
    vk::Extent2D extent = swapchain->Extent();
    auto info = vk::FramebufferCreateInfo{}
                    .setAttachments(views)
                    .setWidth(extent.width)
                    .setHeight(extent.height)
                    .setLayers(1)
                    .setRenderPass(*renderpass->Handle());

    vk::Framebuffer frame_buffer = device->Handle()->createFramebuffer(info);
    handle = vk::SharedFramebuffer(frame_buffer, device->Handle());
}

Framebuffer::Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                         ren::RenderPassH renderpass,
                         std::vector<ren::AttachmentDataH> &&att)
    : attachments{std::move(att)} {
    log::Debug("Framebuffer creation with move");
    if (attachments.empty()) {
        log::Warn("Empty attachments array inside framebuffer ctor");
        return;
    }
    std::vector<vk::ImageView> views;
    views.reserve(att.size());
    for (const auto &a : attachments) {
        views.push_back(*a->imageView);
    }
    vk::Extent2D extent = swapchain->Extent();
    auto info = vk::FramebufferCreateInfo{}
                    .setAttachments(views)
                    .setWidth(extent.width)
                    .setHeight(extent.height)
                    .setLayers(1)
                    .setRenderPass(*renderpass->Handle());

    vk::Framebuffer frame_buffer = device->Handle()->createFramebuffer(info);
    handle = vk::SharedFramebuffer(frame_buffer, device->Handle());
}

auto Framebuffer::HandleName() const noexcept -> std::string {
    return "Framebuffer";
}

ImageContext::ImageContext(FramebufferH framebuffer)
    : framebuffer{std::move(framebuffer)} {}

FrameContext::FrameContext(CommandBufferH commandBuffer, SyncDataH syncData)
    : commandBuffer{std::move(commandBuffer)}, syncData{std::move(syncData)} {}

// FrameContext::FrameContext(FrameContext &&other) noexcept
//     : framebuffer{std::move(other.framebuffer)},
//       commandBuffer{std::move(other.commandBuffer)},
//       syncData{std::move(other.syncData)} {
//     log::Debug("Moving frame context");
// }

// clang-format off
RenderContext::RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                             ren::RenderPassH renderpass,
                             ren::CommandPoolH command_pool,
                             size_t frames,
                             const std::vector<ren::AttachmentH>& attachments) 
    : currentImageIndex{ 0 } 
    , currentFrameIndex{ 0 } 
    , framesInFlight{ frames } 
{
    
    log::Debug("Creating RenderContext...");
    CreateFrameContexts(device, command_pool, frames);
    CreateImageContexts(device, swapchain, renderpass, attachments);
    log::Debug("Context was build");
}

void RenderContext::CleanUp()
{ 
    frameContexts.clear();
}


auto RenderContext::GetFrameContext(size_t frame) -> FrameContextH  { return frameContexts[frame]; }
auto RenderContext::GetImageContext(size_t image) -> ImageContextH  { return imageContexts[image]; }
auto RenderContext::CurrentImageIndex() const noexcept -> size_t { return currentImageIndex; }
auto RenderContext::CurrentFrameIndex() const noexcept -> size_t { return currentFrameIndex; }
auto RenderContext::CurrentCommandBuffer() noexcept -> CommandBufferH { 
    auto frame_ctx = GetFrameContext(currentFrameIndex);
    return frame_ctx->commandBuffer;
}
auto RenderContext::CurrentFramebuffer() noexcept -> FramebufferH { 
    auto image_ctx = GetImageContext(currentImageIndex);
    return image_ctx->framebuffer;
}

auto RenderContext::BeginFrame(DeviceH device, SwapchainH swapchain) -> bool { 
    const auto timeout = std::numeric_limits<uint64_t>::max();
    auto frame_ctx = GetFrameContext(currentFrameIndex);
    WaitFence(device, frame_ctx, timeout);
    bool sts = GetNewImage(device, swapchain, frame_ctx, timeout);
    // auto image_ctx = GetImageContext(currentImageIndex);
    ResetFences(device, frame_ctx);
    BeginCommandBuffer(frame_ctx);
    return sts;
}

auto RenderContext::EndFrame(DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain) -> bool { 
    const auto timeout = std::numeric_limits<uint64_t>::max();
    auto frame_ctx = GetFrameContext(currentFrameIndex);
    EndCommandBuffer(frame_ctx);
    SubmitGraphics(frame_ctx, physical_device);
    bool sts = SubmitPresent(frame_ctx, physical_device, swapchain);
    // WaitFences(device, frame_ctx, timeout);
    currentFrameIndex = (currentFrameIndex + 1) % framesInFlight;
    return sts;
}

// clang-format off
void RenderContext::CreateFrameContexts(
    ren::DeviceH device, 
    ren::CommandPoolH command_pool, 
    size_t frames) {
    frameContexts.reserve(frames);
    for (size_t frame = 0; frame < frames; ++frame) {
        log::Debug("[{}] Creating sync-data", frame);
        auto syncdata = std::make_shared<ren::SyncData>(device);

        log::Debug("[{}] Creating command_buffer", frame);
        auto command_buf = std::make_shared<ren::CommandBuffer>(device, command_pool);

        log::Debug("[{}] Creating render-context data", frame);
        frameContexts.push_back(std::make_shared<ren::FrameContext>(
            std::move(command_buf),
            std::move(syncdata)
        ));
    }
}

void RenderContext::ClearFrameContexts() { 
    frameContexts.clear();
}

void RenderContext::CreateImageContexts(ren::DeviceH device, ren::SwapchainH swapchain,
                         ren::RenderPassH renderpass,
                         const std::vector<ren::AttachmentH> &attachments) { 
    imageContexts.clear();
    for (size_t image = 0; image < swapchain->Images().size(); ++image) {  
        std::vector<ren::AttachmentDataH> attachement_datas;
        for (const auto &att : attachments) {
            auto attachemnt_data = std::make_shared<ren::AttachmentData>(
                att->CreateData(image, device)
            );
            attachement_datas.push_back( std::move(attachemnt_data) );
        }
        auto framebuffer = std::make_shared<ren::Framebuffer>(
            device, swapchain, renderpass, std::move(attachement_datas)
        );
        imageContexts.push_back(std::make_shared<ImageContext>(std::move(framebuffer)));
    }
}

void RenderContext::ClearImageContexts() { 
    imageContexts.clear(); 
}


void RenderContext::WaitFences(DeviceH device, SwapchainH swapchain) {
    for (auto& ctx : frameContexts) { 
        auto acquire_result = device->Handle()->acquireNextImageKHR(
            swapchain->CHandle(), 
            UINT64_MAX,
            ctx->syncData->availableSemaphore.get(),
            VK_NULL_HANDLE,
            &currentImageIndex
        );
    }


    // for (auto& ctx : frameContexts) { 
    //     auto _ = device->Handle()->waitForFences(ctx->syncData->frameFence.get(), true, UINT64_MAX);
    // }
}

auto RenderContext::GetNewImage(DeviceH device, SwapchainH swapchain, FrameContextH frame_ctx, uint64_t timeout) -> bool { 
    auto acquire_result = device->Handle()->acquireNextImageKHR(
        swapchain->CHandle(), 
        timeout,
        frame_ctx->syncData->availableSemaphore.get(),
        VK_NULL_HANDLE,
        &currentImageIndex
    );

    if (acquire_result  == vk::Result::eErrorOutOfDateKHR) { 
        log::Error("Need to recreate swachain... [1]");
        return true;
    } else if (acquire_result != vk::Result::eSuccess && 
               acquire_result != vk::Result::eSuboptimalKHR) { 
        log::FatalExit("Failed to acquire swapchain image...");
    }
    return false;
}

auto RenderContext::ResetFences(DeviceH device, FrameContextH frame_ctx) -> void { 
    frame_ctx->commandBuffer->Handle()->reset();
    device->Handle()->resetFences(std::vector{ 
        frame_ctx->syncData->frameFence.get()
    });
}

void RenderContext::BeginCommandBuffer(FrameContextH frame_ctx) { 
    auto command_buffer_begin_info = vk::CommandBufferBeginInfo{} 
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
        ;
    frame_ctx->commandBuffer->Handle()->begin(command_buffer_begin_info);
}


void RenderContext::EndCommandBuffer(FrameContextH frame_ctx) { 
    frame_ctx->commandBuffer->Handle()->end();  
}

void RenderContext::SubmitGraphics(FrameContextH frame_ctx, PhysicalDeviceH physical_device) { 
    auto wait_semaphores = std::array { frame_ctx->syncData->availableSemaphore.get() };
    auto wait_stages = vk::PipelineStageFlags { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    auto signal_semaphores = std::array { frame_ctx->syncData->finishSemaphore.get() };
    auto command_buffers = std::array { frame_ctx->commandBuffer->CHandle() };
    auto submit_info = vk::SubmitInfo{}
        .setWaitSemaphores(wait_semaphores)
        .setWaitDstStageMask(wait_stages)
        .setCommandBuffers(command_buffers) 
        .setSignalSemaphores(signal_semaphores)
        ;
    // log::Debug("GraphicsQueue={}", physical_device->QueueIndex(vk::QueueFlagBits::eGraphics));
    auto& graphics_queue = physical_device->GraphicsQueue();
    graphics_queue.submit(submit_info, frame_ctx->syncData->frameFence.get());
}

bool RenderContext::SubmitPresent(FrameContextH frame_ctx, PhysicalDeviceH physical_device, SwapchainH swapchain) { 
    auto swapchains = std::array { swapchain->CHandle() };
    auto signal_semaphores = std::array { frame_ctx->syncData->finishSemaphore.get() };
    auto present_info = vk::PresentInfoKHR{}
        .setWaitSemaphores(signal_semaphores)
        .setSwapchains(swapchains)
        .setImageIndices(currentImageIndex)
        ;

    // log::Debug("PresentQueue={}", physical_device->PresentQueueIndex());
    auto& present_queue = physical_device->PresentQueue();
    auto res_qp = present_queue.presentKHR(&present_info);

    if (res_qp == vk::Result::eErrorOutOfDateKHR || res_qp == vk::Result::eSuboptimalKHR) {
        log::Info("Need to recreate swachain 2...");
        return true;
    } else if (res_qp != vk::Result::eSuccess) { 
        log::FatalExit("Failed to present KHR...");
    }
    return false;
}

void RenderContext::WaitFence(DeviceH device, FrameContextH frame_ctx, uint64_t timeout) { 
    auto _ = device->Handle()->waitForFences(frame_ctx->syncData->frameFence.get(), true, timeout);
}


} // namespace nj::ren
