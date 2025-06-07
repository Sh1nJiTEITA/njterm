#pragma once
#include <vulkan/vulkan_shared.hpp>
#ifndef NJ_PHYSICAL_DEVICE_H
#define NJ_PHYSICAL_DEVICE_H

namespace nj::ren {

class PhysicalDevice {
  public:
    PhysicalDevice() = default;

  private:
    vk::SharedPhysicalDevice phDevice;
};

}; // namespace nj::ren

#endif
