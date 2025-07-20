#pragma once
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_device.h"
#include "nj_handle.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class DescriptorPool : VulkanObject<vk::DescriptorPool> {
  public:
    DescriptorPool(ren::DeviceH device);
    auto HandleName() const noexcept -> std::string override;
};

} // namespace nj::ren

#endif
