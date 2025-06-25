#include "nj_command_pool.h"

namespace nj::ren {

// clang-format off
CommandPool::CommandPool(ren::DeviceH device, ren::PhysicalDeviceH phDevice) {
    auto info = vk::CommandPoolCreateInfo{}
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(phDevice->PresentQueueIndex())
    ;
    auto raw = device->Handle()->createCommandPool(info);
    handle = vk::SharedCommandPool(raw, device->Handle());
}


CommandPool::~CommandPool() { 
    handle.reset();
}
// clang-format on

auto CommandPool::HandleName() const noexcept -> std::string {
    return "CommandPool";
}

} // namespace nj::ren
