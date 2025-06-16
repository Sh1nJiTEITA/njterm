#include "nj_device.h"
#include "nj_build_device.h"

namespace nj::ren {

Device::Device(ren::InstanceH instance, ren::PhysicalDeviceH phDevice,
               vk::SharedSurfaceKHR surface) {
    handle = build::Build<vk::Device>(instance, phDevice, surface);
}

} // namespace nj::ren
