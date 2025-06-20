#pragma once
#ifndef NJ_COMMAND_POOL_H
#define NJ_COMMAND_POOL_H

#include "nj_device.h"
#include "nj_handle.h"
#include "nj_physical_device.h"
#include <memory>

namespace nj::ren {

class CommandPool : public VulkanObject<vk::CommandPool> {
  public:
    CommandPool(ren::DeviceH, ren::PhysicalDeviceH);
    auto HandleName() const noexcept -> std::string override;
};
using CommandPoolH = std::shared_ptr<CommandPool>;

} // namespace nj::ren

#endif
