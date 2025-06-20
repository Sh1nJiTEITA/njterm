#pragma once
#ifndef NJ_SWAP_CHAIN_H
#define NJ_SWAP_CHAIN_H

#include "nj_device.h"
#include "nj_handle.h"
#include <memory>

namespace nj::ren {

class Swapchain : public VulkanObject<vk::SwapchainKHR> {
  public:
    Swapchain(ren::PhysicalDeviceH phDevice, ren::DeviceH device,
              vk::SharedSurfaceKHR surface, vk::Extent2D ext);
    //! Current swapchain extent (simular to window size)
    auto Extent() -> vk::Extent2D;

    //! Current swapchain format (pixel?)
    auto Format() -> vk::Format;

    //! Mostly internal method to get swapchain images.
    //! WARN: Must be callled once on creation.
    //! Just updates inner array of shared images;
    auto UpdateImages(ren::DeviceH device) -> void;

    //! Get access to stored swapchain images
    auto Images() -> const std::vector<vk::SharedImage> &;

    auto HandleName() const noexcept -> std::string override;

  private:
    std::vector<vk::SharedImage> images;
    vk::Format format;
    const vk::Extent2D extent;
};
using SwapchainH = std::shared_ptr<Swapchain>;

} // namespace nj::ren

#endif
