#pragma once
#ifndef NJ_DEVICE_H
#define NJ_DEVICE_H

#include "nj_handle.h"
#include <memory>

namespace nj::ren {

class Device : public VulkanObject<vk::Device> {
  public:
    Device() = default;
};
using DeviceH = std::shared_ptr<Device>;

} // namespace nj::ren

#endif
