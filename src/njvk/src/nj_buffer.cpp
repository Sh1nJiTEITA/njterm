#include "nj_buffer.h"
#include "nj_log_scope.h"
#include "njlog.h"
#include "njvklog.h"
#include "njvkutils.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off
Buffer::Buffer(ren::DeviceH device, ren::AllocatorH allocator, size_t alloc_size,
           vk::BufferUsageFlags vk_flags, VmaMemoryUsage vma_usage,
           VmaAllocationCreateFlags flags) 
    : AllocationUnit(allocator)
    , initialAllocationSize{ alloc_size } 
{
    DEBUG_SCOPE_A("Creating new vkBuffer with id={}", id);
    log::DebugA("Buffer initial size={}", alloc_size);

    auto buffer_info = vk::BufferCreateInfo{}
      .setSize(alloc_size)
      .setUsage(vk_flags)
      .setPNext(nullptr)
    ;
    auto vma_info = VmaAllocationCreateInfo { 
      .flags = flags,
      .usage = vma_usage
    };

    handle = std::make_unique< vk::Buffer >();

    VkBuffer pBuffer {};

    log::DebugA("Creating VMA buffer");
    auto res = static_cast<vk::Result>(vmaCreateBuffer(
        allocator->Handle(),
        &static_cast<VkBufferCreateInfo&>(buffer_info),
        &vma_info,
        &pBuffer,
        &allocation,
        &allocationInfo
    ));
    *handle = pBuffer; 

    log::DebugA("VMA creation done");
    log::DebugA("Offset={}", allocationInfo.offset);
    log::DebugA("pName={}", allocationInfo.pName ? allocationInfo.pName : "");
    log::DebugA("Size={}", allocationInfo.size);

    // vmaDestroyBuffer(*allocator->CHandle(), pBuffer, allocation);
    log::CheckCall(res, "Cant create vulkan buffer");
}

Buffer::~Buffer() { 
    DEBUG_SCOPE_A("Deleting vkBuffer with id={}", id);
    vmaDestroyBuffer(allocator->Handle(), *handle, allocation);
}

// clang-format on

auto Buffer::Map() -> void* {
    void* p;
    vmaMapMemory(allocator->Handle(), allocation, &p);
    return p;
}

auto Buffer::Unmap() -> void {
    vmaUnmapMemory(allocator->Handle(), allocation);
}

auto Buffer::HandleName() const noexcept -> std::string { return "Buffer"; }

auto Buffer::InitialSize() const noexcept -> size_t {
    return initialAllocationSize;
}

} // namespace nj::ren
