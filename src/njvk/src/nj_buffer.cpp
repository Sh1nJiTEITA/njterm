#include "nj_buffer.h"
#include "njlog.h"
#include "njvklog.h"

namespace nj::ren {

// clang-format off
Buffer::Buffer(ren::DeviceH device, ren::AllocatorH allocator, size_t alloc_size,
           vk::BufferUsageFlags vk_flags, VmaMemoryUsage vma_usage,
           VmaAllocationCreateFlags flags) 
    : allocator{ allocator } 
{
    log::Debug("Creating new vkBuffer...");

    auto buffer_info = vk::BufferCreateInfo{}
      .setSize(alloc_size)
      .setUsage(vk_flags)
      .setPNext(nullptr)
    ;
    auto vma_info = VmaAllocationCreateInfo { 
      .flags = flags,
      .usage = vma_usage
    };

    VkBuffer buf{};

    auto res = static_cast<vk::Result>(vmaCreateBuffer(
        *allocator->CHandle(),
        &static_cast<VkBufferCreateInfo&>(buffer_info),
        &vma_info,
        &buf,
        &allocation,
        &allocationInfo
    ));
    log::CheckCall(res, "Cant create vulkan buffer");
    handle = vk::SharedBuffer(buf, device->Handle(), {});
}

Buffer::~Buffer() { 
    log::Debug("Deleting vkBuffer...");
    // vmaDestroyBuffer(*allocator->CHandle(), handle.get(), allocation);
}

// clang-format on

auto Buffer::Allocation() -> VmaAllocation & { return allocation; }

auto Buffer::Map() -> void * {
    void *p;
    vmaMapMemory(*allocator->CHandle(), allocation, &p);
    return p;
}

auto Buffer::Unmap() -> void {
    vmaUnmapMemory(*allocator->CHandle(), allocation);
}

auto Buffer::Size() -> size_t { return allocationInfo.size; }

auto Buffer::HandleName() const noexcept -> std::string { return "Buffer"; }

} // namespace nj::ren
