#pragma once
#ifndef NJ_BUFFER_H
#define NJ_BUFFER_H

#include "nj_allocator.h"
#include "nj_handle.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class Buffer : public AllocationUnit, public VulkanObjectManual<vk::Buffer> {
  public:
    Buffer(ren::DeviceH device, ren::AllocatorH allocator, size_t alloc_size,
           vk::BufferUsageFlags vk_flags, VmaMemoryUsage vma_usage,
           VmaAllocationCreateFlags flags);
    ~Buffer();

    auto Map() -> void *;
    auto Unmap() -> void;
    auto HandleName() const noexcept -> std::string override;
    auto InitialSize() const noexcept -> size_t {
        return initialAllocationSize;
    }

  protected:
    const size_t initialAllocationSize;
};

} // namespace nj::ren

#endif
