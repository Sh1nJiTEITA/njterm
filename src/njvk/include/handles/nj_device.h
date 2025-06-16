#pragma once
#ifndef NJ_DEVICE_H
#define NJ_DEVICE_H

#include "nj_handle.h"
#include "nj_instance.h"
#include "nj_physical_device.h"
#include <memory>

namespace nj::ren {

class Device : public VulkanObject<vk::Device> {
  public:
    Device(ren::InstanceH, ren::PhysicalDeviceH, vk::SharedSurfaceKHR);
};
using DeviceH = std::shared_ptr<Device>;

} // namespace nj::ren

#endif
