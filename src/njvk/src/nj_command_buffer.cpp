#include "nj_command_buffer.h"
#include "nj_command_pool.h"
#include "njlog.h"
#include "njvklog.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

// clang-format off
CommandBuffer::CommandBuffer(ren::DeviceH device,
                             ren::CommandPoolH command_pool)
{
    auto info = vk::CommandBufferAllocateInfo{}
                    .setCommandPool(command_pool->Handle())
                    .setLevel(vk::CommandBufferLevel::ePrimary)
                    .setCommandBufferCount(1);

    handle = std::move(device->Handle().allocateCommandBuffersUnique(info).front());
}

auto CommandBuffer::HandleName() const noexcept -> std::string {
    return "CommandBuffer";
}

} // namespace nj::ren
