#pragma once
#ifndef NJ_DEVICE_H
#define NJ_DEVICE_H

#include "handles/nj_builder.h"

namespace nj::build {

template <> class Builder<vk::Device> {
  public:
    Builder(vk::SharedInstance instance, vk::SharedPhysicalDevice);

    using Handle = vk::SharedDevice;
    Handle Build();

  private:
    vk::SharedInstance inst;
    vk::SharedPhysicalDevice phDevice;
};

} // namespace nj::build

#endif
