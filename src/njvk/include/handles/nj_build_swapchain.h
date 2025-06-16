#pragma once
#ifndef NJ_BUILD_SWAP_CHAIN_H
#define NJ_BUILD_SWAP_CHAIN_H

#include "handles/nj_swapchain.h"
#include "nj_builder.h"
#include "nj_device.h"
#include "nj_physical_device.h"

namespace nj::build {

template <> class Builder<vk::SwapchainKHR> {
  public:
    Builder(ren::PhysicalDeviceH phDevice, ren::DeviceH device,
            vk::SharedSurfaceKHR surface, uint32_t width, uint32_t height);

    using Handle = vk::SharedSwapchainKHR;
    Handle Build();

    vk::Extent2D Extent();
    vk::Format Format();

  private:
    vk::Extent2D extent;
    vk::Format format;
    vk::SharedSurfaceKHR surface;
    ren::PhysicalDeviceH phDevice;
    ren::DeviceH device;
    ren::VarHandles h;
};

// template <> class Builder<ren::Swapchain> {
//   public:
//     Builder(ren::PhysicalDeviceH phDevice, ren::DeviceH device,
//             vk::SharedSurfaceKHR surface, uint32_t width, uint32_t height);
//
//     using Handle = ren::SwapchainH;
//     Handle Build();
//
//   private:
//     vk::Extent2D extent;
//     vk::SharedSurfaceKHR surface;
//     ren::PhysicalDeviceH phDevice;
//     ren::DeviceH device;
//     ren::VarHandles h;
// };

} // namespace nj::build

#endif
