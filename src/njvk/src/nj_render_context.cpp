#include "nj_render_context.h"
#include "nj_build_framebuffer.h"

namespace nj::ren {

vk::SharedFramebuffer &FrameData::Framebuffer() { return framebuffer; }

auto FrameData::Attachement(size_t idx) -> const ren::AttachmentDataH & {
    return attachments.at(idx);
}

FrameData::FrameData(ren::DeviceH device, ren::SwapchainH swapchain,
                     const std::vector<ren::AttachmentDataH> &att)
    : attachments{att} {

    std::vector<vk::SharedImageView> views;
    views.reserve(att.size());
    for (auto &a : att) {
        views.push_back(a->imageView);
    }

    framebuffer = build::Build<vk::Framebuffer>(device, swapchain, views);
}

} // namespace nj::ren
