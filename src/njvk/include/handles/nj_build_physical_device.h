#pragma once
#include <memory>
#ifndef NJ_BUILD_PHYSICAL_DEVICE_H
#define NJ_BUILD_PHYSICAL_DEVICE_H

#include "nj_builder.h"
#include "nj_physical_device.h"

namespace nj::build {

template <> class Builder<vk::PhysicalDevice> {
  public:
    Builder(vk::SharedInstance instance);

    using Handle = vk::SharedPhysicalDevice;
    Handle Build();

  private:
    vk::SharedInstance inst;
};

template <> class Builder<ren::PhysicalDevice> {
  public:
    Builder(vk::SharedInstance instance);

    using Handle = std::shared_ptr<ren::PhysicalDevice>;
    Handle Build();

  private:
    vk::SharedInstance inst;
};

} // namespace nj::build

#endif
