#pragma once
#ifndef NJ_BUILD_DEVICE_H
#define NJ_BUILD_DEVICE_H

#include "nj_builder.h"
#include "nj_device.h"
#include "nj_instance.h"
#include "nj_physical_device.h"
#include <memory>

namespace nj::build {

template <> class Builder<vk::Device> {
  public:
    Builder(ren::InstanceH, ren::PhysicalDeviceH, vk::SharedSurfaceKHR);

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
    ren::InstanceH inst;
    ren::PhysicalDeviceH phDevice;
    vk::SharedSurfaceKHR surface;
    ren::VarHandles h;
};

template <> class Builder<ren::Device> {
  public:
    Builder(ren::InstanceH, ren::PhysicalDeviceH, vk::SharedSurfaceKHR);

    using Handle = std::shared_ptr<ren::Device>;
    Handle Build();

  private:
    ren::InstanceH inst;
    ren::PhysicalDeviceH phDevice;
    vk::SharedSurfaceKHR surface;
    ren::VarHandles h;
};

} // namespace nj::build

#endif
