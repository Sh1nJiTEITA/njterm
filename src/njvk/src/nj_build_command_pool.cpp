#include "nj_build_command_pool.h"
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {

using BCommandPool = Builder<vk::CommandPool>;

BCommandPool::Builder(ren::DeviceH device, ren::PhysicalDeviceH phDevice)
    : device{device}, phDevice{phDevice} {}

// clang-format off
BCommandPool::Handle BCommandPool::Build() {
    auto info = vk::CommandPoolCreateInfo{}
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(phDevice->PresentQueueIndex())
    ;
    auto raw = device->Handle()->createCommandPool(info);
    return vk::SharedCommandPool(raw, device->Handle());
}
// clang-format on

} // namespace nj::build
