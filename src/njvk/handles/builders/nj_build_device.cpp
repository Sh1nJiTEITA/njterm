#include "nj_build_device.h"
#include <ranges>

namespace nj::build {

using BDevice = Builder<vk::Device>;

BDevice::Builder(vk::SharedInstance instance, vk::SharedPhysicalDevice phDevice)
    : inst{instance}, phDevice{phDevice} {}

BDevice::Handle BDevice::Build() {
    auto properties = phDevice->getQueueFamilyProperties();
    // auto unique_indices =
    //     properties |
    //     std::views::transform([](const vk::QueueFamilyProperties &prop) {
    //       return prop.
    // }
    //
    //     );
}

} // namespace nj::build
