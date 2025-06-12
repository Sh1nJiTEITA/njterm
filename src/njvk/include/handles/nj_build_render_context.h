#pragma once
#ifndef NJ_BUILD_FRAME_DATA_H
#define NJ_BUILD_FRAME_DATA_H

#include "nj_builder.h"
#include "nj_device.h"
#include "nj_render_context.h"
#include "nj_swapchain.h"

namespace nj::build {

template <> class Builder<ren::FrameData> {
  public:
    Builder(ren::DeviceH device, vk::SharedImage swapchain_image,
            vk::Format format, vk::Extent2D extent,
            const std::vector<vk::SharedImageView> &att);

    using Handle = ren::FrameDataH;
    Handle Build();

    auto BuildImage() -> vk::SharedImage;
    auto BuildView() -> vk::SharedImageView;
    auto BuildFramebuffer() -> vk::SharedFramebuffer;

  private:
    ren::FrameDataH data;
    ren::DeviceH device;
    vk::Extent2D extent;
    vk::SharedImage image;
    vk::Format format;
    std::vector<vk::SharedImageView> attachments;
    nj::ren::VarHandles h;
};

template <> class Builder<ren::RenderContext> {
  public:
    Builder(ren::DeviceH device, ren::SwapchainH swapchain,
            const std::vector<vk::SharedImageView> &att);
    Builder(ren::DeviceH device, ren::SwapchainH swapchain);

    using Handle = ren::RenderContextH;
    Handle Build();

  private:
    ren::DeviceH device;
    ren::SwapchainH swapchain;
    std::vector<vk::SharedImageView> attachments;
};

}; // namespace nj::build

#endif
