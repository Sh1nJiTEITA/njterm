#pragma once
#ifndef NJ_PHYSICAL_DEVICE_H
#define NJ_PHYSICAL_DEVICE_H

#include "handles/nj_builder.h"

namespace nj::build {

template <> class Builder<vk::PhysicalDevice> {
  public:
    Builder(vk::SharedInstance instance);

    using Handle = vk::SharedPhysicalDevice;
    Handle Build();

  private:
    vk::SharedInstance inst;
};

} // namespace nj::build

#endif
