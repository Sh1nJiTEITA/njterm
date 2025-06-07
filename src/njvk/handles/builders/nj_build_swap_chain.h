#pragma once
#ifndef NJ_BUILD_SWAP_CHAIN_H
#define NJ_BUILD_SWAP_CHAIN_H

#include "handles/nj_builder.h"
#include "handles/nj_swapchain.h"
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {

template <> class Builder<vk::SwapchainKHR> {
  public:
    Builder(vk::SharedPhysicalDevice phDevice, vk::SharedDevice device,
            vk::SharedSurfaceKHR surface, uint32_t width, uint32_t height);

    using Handle = vk::SharedSwapchainKHR;
    Handle Build();

    vk::Extent2D Extent();

  private:
    vk::Extent2D extent;

    vk::SharedSurfaceKHR surface;
    vk::SharedPhysicalDevice phDevice;
    vk::SharedDevice device;
};

template <> class Builder<ren::Swapchain> {
  public:
    Builder(vk::SharedPhysicalDevice phDevice, vk::SharedDevice device);

    using Handle = ren::Swapchain;
    Handle Build();

  private:
    vk::SharedPhysicalDevice phDevice;
    vk::SharedDevice device;
};

} // namespace nj::build

#endif
