#include "nj_render_context.h"
#include "nj_command_buffer.h"
#include "nj_render_pass.h"
#include "njcon.h"
#include "njlog.h"
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

FrameContext::FrameContext(FramebufferH framebuffer,
                           CommandBufferH commandBuffer, SyncDataH syncData)
    : framebuffer{std::move(framebuffer)}, commandBuffer{commandBuffer},
      syncData{std::move(syncData)} {}

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
                             const std::vector<ren::AttachmentH>& attachments) {
    
    log::Debug("Creating RenderContext...");
    CreateFrameContexts(
        device, swapchain, renderpass, 
        command_pool, frames, attachments
    );
    log::Debug("Context was build");
}

void RenderContext::CleanUp()
{ 
    contexts.clear();
}


auto RenderContext::Context(size_t frame) -> FrameContextH & { 
    return contexts[frame];
}

// clang-format off
void RenderContext::CreateFrameContexts(
    ren::DeviceH device, 
    ren::SwapchainH swapchain, 
    ren::RenderPassH renderpass, 
    ren::CommandPoolH command_pool, 
    size_t frames, 
    const std::vector<ren::AttachmentH> &attachments) {
    
    contexts.reserve(frames);
    for (uint32_t frame = 0; frame < frames; ++frame) {
        log::Debug("[{}] Creating framebuffer", frame);
        std::vector<ren::AttachmentDataH> attachement_datas;
        for (const auto &att : attachments) {
            auto attachemnt_data = std::make_shared<ren::AttachmentData>(
                att->CreateData(frame, device)
            );
            attachement_datas.push_back( std::move(attachemnt_data) );
        }
        auto framebuffer = std::make_shared<ren::Framebuffer>(
            device, swapchain, renderpass, std::move(attachement_datas)
        );
        log::Debug("[{}] Creating sync-data", frame);
        auto syncdata = std::make_shared<ren::SyncData>(device);
        log::Debug("[{}] Creating command_buffer", frame);
        auto command_buf = std::make_shared<ren::CommandBuffer>(device, command_pool);
        log::Debug("[{}] Creating render-context data", frame);
        contexts.push_back(std::make_shared<ren::FrameContext>(
            std::move(framebuffer), 
            std::move(command_buf),
            std::move(syncdata)
        ));
    }
}

} // namespace nj::ren
