#pragma once
#ifndef NJ_SWAP_CHAIN_H
#define NJ_SWAP_CHAIN_H

#include "nj_device.h"
#include "nj_handle.h"
#include "nj_image.h"
#include <memory>

namespace nj::ren {

class Swapchain : public VulkanObjectNative<vk::SwapchainKHR> {
  public:
    Swapchain(PhysicalDeviceH phDevice, DeviceH device, SurfaceH surface,
              vk::Extent2D ext, vk::ImageUsageFlags flags);
    //! Current swapchain extent (simular to window size)
    auto Extent() -> vk::Extent2D;

    //! Current swapchain format (pixel?)
    auto Format() -> vk::Format;

    //! Mostly internal method to get swapchain images.
    //! WARN: Must be callled once on creation.
    //! Just updates inner array of shared images;
    auto UpdateImages(ren::DeviceH device) -> void;

    //! Get access to stored swapchain images
    auto Images() -> const std::vector<ImageH> &;

    auto HandleName() const noexcept -> std::string override;

  private:
    std::vector<ImageH> images;
    vk::Format format;
    vk::Extent2D extent;
};
using SwapchainH = std::shared_ptr<Swapchain>;

} // namespace nj::ren

#endif
