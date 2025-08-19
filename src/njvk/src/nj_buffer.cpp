#include "nj_buffer.h"
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
    log::Debug("Creating new vkBuffer with id={}...", id);

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

    auto res = static_cast<vk::Result>(vmaCreateBuffer(
        allocator->Handle(),
        &static_cast<VkBufferCreateInfo&>(buffer_info),
        &vma_info,
        &pBuffer,
        &allocation,
        &allocationInfo
    ));
    *handle = pBuffer; 

    // vmaDestroyBuffer(*allocator->CHandle(), pBuffer, allocation);
    log::CheckCall(res, "Cant create vulkan buffer");
}

Buffer::~Buffer() { 
    log::Debug("Deleting vkBuffer with id={}...", id);
    vmaDestroyBuffer(allocator->Handle(), *handle, allocation);
}

// clang-format on

auto Buffer::Map() -> void * {
    void *p;
    vmaMapMemory(allocator->Handle(), allocation, &p);
    return p;
}

auto Buffer::Unmap() -> void {
    vmaUnmapMemory(allocator->Handle(), allocation);
}

auto Buffer::HandleName() const noexcept -> std::string { return "Buffer"; }

} // namespace nj::ren
