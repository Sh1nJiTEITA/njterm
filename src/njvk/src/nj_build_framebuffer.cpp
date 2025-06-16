#include "nj_build_framebuffer.h"

namespace nj::build {

using BFramebuffer = Builder<vk::Framebuffer>;

BFramebuffer::Builder(ren::DeviceH device, ren::SwapchainH swapchain,
                      const std::vector<vk::SharedImageView> &views)
    : views{views}, device{device}, swapchain{swapchain}

{}

auto BFramebuffer::Build() -> vk::SharedFramebuffer {
    std::vector<vk::ImageView> transformed_views;
    for (auto &view : views) {
        transformed_views.push_back(*view);
    }
    vk::Extent2D extent = swapchain->Extent();
    auto info = vk::FramebufferCreateInfo{}
                    .setAttachments(transformed_views)
                    .setWidth(extent.width)
                    .setHeight(extent.height)
                    .setLayers(1);
    vk::Framebuffer frame_buffer = device->Handle()->createFramebuffer(info);
    return vk::SharedFramebuffer(frame_buffer, device->Handle());
}

} // namespace nj::build
