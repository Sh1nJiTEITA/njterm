#pragma once
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_allocator.h"
#include "nj_handle.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class Buffer : VulkanObject<vk::Buffer> {
  public:
    Buffer(ren::DeviceH device, ren::AllocatorH allocator, size_t alloc_size,
           vk::BufferUsageFlags vk_flags, VmaMemoryUsage vma_usage,
           VmaAllocationCreateFlags flags);
    ~Buffer();

    auto Allocation() -> VmaAllocation &;
    auto Map() -> void *;
    auto Unmap() -> void;
    auto Size() -> size_t;

    auto HandleName() const noexcept -> std::string override;

  private:
    ren::AllocatorH allocator;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;
};

} // namespace nj::ren

#endif
