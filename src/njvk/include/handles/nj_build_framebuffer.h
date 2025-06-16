#pragma once
#ifndef NJ_BUILD_FRAMEBUFFER_H
#define NJ_BUILD_FRAMEBUFFER_H

#include "nj_builder.h"
#include "nj_device.h"
#include "nj_swapchain.h"

namespace nj::build {

template <> class Builder<vk::Framebuffer> {
  public:
    Builder(ren::DeviceH device, ren::SwapchainH swapchain,
            const std::vector<vk::SharedImageView> &views);

    using Handle = vk::SharedFramebuffer;
    Handle Build();

  private:
    ren::DeviceH device;
    ren::SwapchainH swapchain;
    std::vector<vk::SharedImageView> views;
};

} // namespace nj::build

#endif
