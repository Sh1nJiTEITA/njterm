#include "nj_render_context.h"
#include "nj_render_pass.h"
#include "njcon.h"
#include "njlog.h"

namespace nj::ren {

auto Framebuffer::Attachement(size_t idx) -> const ren::AttachmentDataH & {
    return attachments.at(idx);
}

Framebuffer::Framebuffer(ren::DeviceH device, ren::SwapchainH swapchain,
                         ren::RenderPassH renderpass,
                         const std::vector<ren::AttachmentDataH> &att)
    : attachments{att} {

    std::vector<vk::ImageView> views;
    views.reserve(att.size());
    for (auto &a : att) {
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

// clang-format off
RenderContext::RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                             ren::RenderPassH renderpass,
                             const std::vector<ren::AttachmentH> &attachments) {

    const uint32_t frames = con::Buffering();
    for (uint32_t frame = 0; frame < frames; ++frame) {
        std::vector<ren::AttachmentDataH> attachement_datas;

        for (auto& att : attachments) { 
            auto attachemnt_data = std::make_shared<ren::AttachmentData>( 
                att->CreateData(frame, device)
            );
            attachement_datas.push_back(std::move(attachemnt_data));
        }

        auto framebuffer = std::make_shared<ren::Framebuffer>(
            device, swapchain, renderpass, attachement_datas
        );
        framebuffers.push_back(std::move(framebuffer));
    }
}

} // namespace nj::ren
