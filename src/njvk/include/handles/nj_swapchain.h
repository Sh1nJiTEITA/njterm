#pragma once
#ifndef NJ_SWAP_CHAIN_H
#define NJ_SWAP_CHAIN_H

#include "nj_handle.h"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

namespace nj::ren {

class Swapchain : public VulkanObject<vk::SwapchainKHR> {
  public:
    Swapchain() = default;
};
using SwapchainH = std::shared_ptr<Swapchain>;

} // namespace nj::ren

#endif
