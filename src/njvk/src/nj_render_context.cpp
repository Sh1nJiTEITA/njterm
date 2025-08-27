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
        views.push_back(a->imageView->Handle());
    }
    vk::Extent2D extent = swapchain->Extent();
    auto info = vk::FramebufferCreateInfo{}
                    .setAttachments(views)
                    .setWidth(extent.width)
                    .setHeight(extent.height)
                    .setLayers(1)
                    .setRenderPass(renderpass->Handle());

    handle = device->Handle().createFramebufferUnique(info);
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
        views.push_back(a->imageView->Handle());
    }
    vk::Extent2D extent = swapchain->Extent();
    auto info = vk::FramebufferCreateInfo{}
                    .setAttachments(views)
                    .setWidth(extent.width)
                    .setHeight(extent.height)
                    .setLayers(1)
                    .setRenderPass(renderpass->Handle());

    handle = device->Handle().createFramebufferUnique(info);
}

auto Framebuffer::HandleName() const noexcept -> std::string {
    return "Framebuffer";
}

ImageContext::ImageContext(FramebufferH framebuffer)
    : framebuffer{std::move(framebuffer)} {}

FrameContext::FrameContext(CommandBufferH commandBuffer, SyncDataH syncData)
    : commandBuffer{std::move(commandBuffer)}, syncData{std::move(syncData)} {}

// clang-format off
RenderContext::RenderContext(DeviceH device, SwapchainH swapchain,
                             RenderPassH renderpass, CommandPoolH command_pool,
                             size_t frames, size_t frame_objects_mode, 
                             const std::vector<ren::AttachmentH> &att) 
    : currentImageIndex{ 0 } 
    , currentFrameIndex{ 0 } 
    , framesInFlight{ frames } 
    , frameObjectsMode{ frame_objects_mode } 
    , swapchainImages{ swapchain->Images().size() } 
{
    
    log::Debug("Creating RenderContext...");
    CreateCmds(device, command_pool);
    CreateSyncDatas(device);
    CreateFramebuffers(device, swapchain, renderpass, att);
    log::Debug("Context was build");
}

#define _NJ_CANT_GET_CURRENT(NAME) { log::FatalExit("Cant get current" NAME ". Invalid frameObjectsMode"); return 0; } 
#define _NJ_CANT_GET_CURRENT_2(NAME) { log::FatalExit("Cant get current" NAME ". Invalid frameObjectsMode"); } 

auto RenderContext::CurrentImageIndex() const noexcept -> size_t { return currentImageIndex; }
auto RenderContext::CurrentFrameIndex() const noexcept -> size_t { return currentFrameIndex; }
auto RenderContext::CurrentCommandBuffer() noexcept -> CommandBufferH { 
    // switch (frameObjectsMode) {
    //     case 0: return cmds[currentFrameIndex]; break;
    //     case 1: return cmds[currentImageIndex]; break;
    //     default: _NJ_CANT_GET_CURRENT("CommandBuffer");
    // };
    return cmds[currentFrameIndex];
}
auto RenderContext::CurrentFramebuffer() noexcept -> FramebufferH { 
    return framebuffers[currentImageIndex];
}
auto RenderContext::CurrentSyncData() noexcept -> SyncDataH { 
    switch (frameObjectsMode) {
        case 0: return syncDatas[currentFrameIndex]; break;
        case 1: return syncDatas[currentImageIndex]; break;
        default: _NJ_CANT_GET_CURRENT("SyncData");
    };
}

auto RenderContext::BeginFrame(DeviceH device, SwapchainH swapchain) -> bool { 
    constexpr auto timeout = std::numeric_limits<uint64_t>::max();
    // WaitFence(device, timeout);
    bool sts = GetNewImage(device, swapchain, timeout);
    ResetFences(device);
    BeginCommandBuffer();
    return sts;
}

auto RenderContext::EndFrame(DeviceH device, PhysicalDeviceH physical_device, SwapchainH swapchain) -> bool { 
    constexpr auto timeout = std::numeric_limits<uint64_t>::max();
    EndCommandBuffer();
    SubmitGraphics(physical_device);
    bool sts = SubmitPresent(physical_device, swapchain);
    // WaitFences(device, frame_ctx, timeout);
    WaitFence(device, timeout);
    UpdateFrameValue();
    return sts;
}

// clang-format off
void RenderContext::CreateCmds(ren::DeviceH device, ren::CommandPoolH command_pool) {
    size_t count;
    switch (frameObjectsMode) {
        case 0: count = framesInFlight; break;
        case 1: count = swapchainImages; break;
        default: _NJ_CANT_GET_CURRENT_2("CreateFrameContexts.count");
    };

    cmds.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        log::Debug("[{}] Creating command_buffer", i);
        cmds.push_back(std::make_shared<ren::CommandBuffer>(device, command_pool));
    }
}

void RenderContext::CreateSyncDatas(ren::DeviceH device) {
    size_t count;
    switch (frameObjectsMode) {
        case 0: count = framesInFlight; break;
        case 1: count = swapchainImages; break;
        default: _NJ_CANT_GET_CURRENT_2("CreateFrameContexts.count");
    };

    syncDatas.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        log::Debug("[{}] Creating sync-data", i);
        syncDatas.push_back(std::make_shared<ren::SyncData>(device));
    }
}

void RenderContext::ClearSyncDatas() { 
    syncDatas.clear();
}

void RenderContext::ClearCmds() { 
    cmds.clear();
}

void RenderContext::CreateFramebuffers(ren::DeviceH device, ren::SwapchainH swapchain,
                         ren::RenderPassH renderpass,
                         const std::vector<ren::AttachmentH> &attachments) { 
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
        framebuffers.push_back(std::move(framebuffer));
    }
}

void RenderContext::ClearFramebuffers() { 
    framebuffers.clear();
}

auto RenderContext::GetNewImage(DeviceH device, SwapchainH swapchain, uint64_t timeout) -> bool { 
    auto sync_data = CurrentSyncData();
    auto acquire_result = device->Handle().acquireNextImageKHR(
        swapchain->CHandle(), 
        timeout,
        sync_data->availableSemaphore.get(),
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

auto RenderContext::ResetFences(DeviceH device) -> void { 
    auto cmd = CurrentCommandBuffer();
    cmd->Handle().reset();
    auto sync_data = CurrentSyncData();
    device->Handle().resetFences(std::vector{ 
        sync_data->frameFence.get()
    });
}

void RenderContext::BeginCommandBuffer() { 
    auto cmd = CurrentCommandBuffer();
    auto command_buffer_begin_info = vk::CommandBufferBeginInfo{} 
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
        ;
    cmd->Handle().begin(command_buffer_begin_info);
}


void RenderContext::EndCommandBuffer() { 
    auto cmd = CurrentCommandBuffer();
    cmd->Handle().end();  
}

void RenderContext::SubmitGraphics(PhysicalDeviceH physical_device) { 
    auto sync_data = CurrentSyncData();
    auto cmd = CurrentCommandBuffer();
    auto wait_semaphores = std::array { sync_data->availableSemaphore.get() };
    auto wait_stages = vk::PipelineStageFlags { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    auto signal_semaphores = std::array { sync_data->finishSemaphore.get() };
    auto command_buffers = std::array { cmd->Handle() };
    auto submit_info = vk::SubmitInfo{}
        .setWaitSemaphores(wait_semaphores)
        .setWaitDstStageMask(wait_stages)
        .setCommandBuffers(command_buffers) 
        .setSignalSemaphores(signal_semaphores)
        ;
    // log::Debug("GraphicsQueue={}", physical_device->QueueIndex(vk::QueueFlagBits::eGraphics));
    auto& graphics_queue = physical_device->GraphicsQueue();
    graphics_queue.submit(submit_info, sync_data->frameFence.get());
}

bool RenderContext::SubmitPresent(PhysicalDeviceH physical_device, SwapchainH swapchain) { 
    auto sync_data = CurrentSyncData();
    auto swapchains = std::array { swapchain->Handle() };
    auto signal_semaphores = std::array { sync_data->finishSemaphore.get() };
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

void RenderContext::WaitFence(DeviceH device, uint64_t timeout) { 
    auto sync_data = CurrentSyncData();
    auto _ = device->Handle().waitForFences(sync_data->frameFence.get(), true, timeout);
}

void RenderContext::UpdateFrameValue() { 
    currentFrameIndex = (currentFrameIndex + 1) % framesInFlight;
}


} // namespace nj::ren
