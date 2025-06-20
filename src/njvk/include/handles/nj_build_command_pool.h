#pragma once
#ifndef NJ_BUILD_COMMAND_POOL_H
#define NJ_BUILD_COMMAND_POOL_H

#include "nj_builder.h"
#include "nj_device.h"
#include "nj_physical_device.h"

namespace nj::build {

//! Creates unique ptr!
template <> class Builder<vk::CommandPool> {
  public:
    Builder(ren::DeviceH device, ren::PhysicalDeviceH phDevice);

    using Handle = vk::SharedCommandPool;
    Handle Build();

  private:
    ren::DeviceH device;
    ren::PhysicalDeviceH phDevice;
};

} // namespace nj::build

#endif
