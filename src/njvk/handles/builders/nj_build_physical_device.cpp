#include "nj_build_physical_device.h"

namespace nj::build {

using BPhysicalDevice = Builder<vk::PhysicalDevice>;

BPhysicalDevice::Builder(vk::SharedInstance instance) : inst{instance} {}
BPhysicalDevice::Handle BPhysicalDevice::Build() {
    return vk::SharedPhysicalDevice(inst->enumeratePhysicalDevices().front(),
                                    inst);
}

} // namespace nj::build
