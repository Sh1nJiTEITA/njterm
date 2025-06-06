#pragma once
#ifndef NJ_DEVICE_H
#define NJ_DEVICE_H

#include "handles/nj_builder.h"

namespace nj::build {

template <> class Builder<vk::Device> {
  public:
    Builder(vk::SharedInstance, vk::SharedPhysicalDevice, vk::SharedSurfaceKHR);

    using Handle = vk::SharedDevice;
    Handle Build();

    //! Creates queue structs for creating vk::Device
    //! Needed for creation flags/extensions/features are located inside
    //! base builder class with group @vk-settings
    const std::vector<vk::DeviceQueueCreateInfo> &DeviceQueueInfos();

    //! Reordered (resampled) version of DeviceFeatures from @vk-settings
    //! Needed for vk::Device builder
    const std::vector<const char *> &DeviceExtensions();

  private:
    vk::SharedInstance inst;
    vk::SharedPhysicalDevice phDevice;
    vk::SharedSurfaceKHR surface;
    ren::VarHandles h;
};

} // namespace nj::build

#endif
