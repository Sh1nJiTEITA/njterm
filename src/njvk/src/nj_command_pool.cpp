#include "nj_command_pool.h"
#include "nj_build_command_pool.h"

namespace nj::ren {

CommandPool::CommandPool(ren::DeviceH device, ren::PhysicalDeviceH phDevice) {
    handle = build::Build<vk::CommandPool>(device, phDevice);
}

auto CommandPool::HandleName() const noexcept -> std::string {
    return "CommandPool";
}

} // namespace nj::ren
