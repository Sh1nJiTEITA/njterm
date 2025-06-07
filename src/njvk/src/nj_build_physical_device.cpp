#include "nj_build_physical_device.h"

namespace nj::build {

using BPhysicalDevice = Builder<vk::PhysicalDevice>;

BPhysicalDevice::Builder(vk::SharedInstance instance) : inst{instance} {}
BPhysicalDevice::Handle BPhysicalDevice::Build() {
    return vk::SharedPhysicalDevice(inst->enumeratePhysicalDevices().front(),
                                    inst);
}

using SBPhysicalDevice = Builder<ren::PhysicalDevice>;
SBPhysicalDevice::Builder(vk::SharedInstance instance) : inst{instance} {}
SBPhysicalDevice::Handle SBPhysicalDevice::Build() {
    auto phDevice = std::make_shared<ren::PhysicalDevice>();
    phDevice->Handle() = Builder<vk::PhysicalDevice>(inst).Build();
    return phDevice;
}

} // namespace nj::build
