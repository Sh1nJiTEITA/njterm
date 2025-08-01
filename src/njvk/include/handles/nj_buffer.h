#pragma once
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_allocator.h"
#include "nj_handle.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class AllocationUnit {

  public:
    AllocationUnit(ren::AllocatorH allocator);
    auto Allocation() -> VmaAllocation &;
    auto Size() -> size_t;
    auto Id() -> uint32_t;

  protected:
    uint32_t id;
    ren::AllocatorH allocator;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;
};

class Buffer : public AllocationUnit, public VulkanObject<vk::Buffer> {
  public:
    Buffer(ren::DeviceH device, ren::AllocatorH allocator, size_t alloc_size,
           vk::BufferUsageFlags vk_flags, VmaMemoryUsage vma_usage,
           VmaAllocationCreateFlags flags);
    ~Buffer();

    auto Map() -> void *;
    auto Unmap() -> void;

    auto HandleName() const noexcept -> std::string override;
};

// clang-format off
class Image : public AllocationUnit, public VulkanObject<vk::Image> {
  public:
    //! Creation of image
    //!
    //! @param device Device to create under
    //! @param allocator Created VMA allocator wrapper handle
    //! @param width Image width
    //! @param height Image height
    //! @param depth Image depth. For default screen usage casually is 1
    //! (simple 2D table)
    //! @param format Image format...
    //! @param usage Special vulkan usage
    //! @param memory_usage VMA easy memory usage flags for allocation
    //! @param vma_flags Other VMA flags
    //! @param init_layout Initial image layout
    //!
    Image(ren::DeviceH device, 
          ren::AllocatorH allocator, 
          size_t width,
          size_t height, 
          size_t depth = 1,
          vk::Format format = vk::Format::eR8G8B8A8Srgb,
          vk::ImageUsageFlags usage = {}, 
          VmaMemoryUsage memory_usage = {},
          VmaAllocationCreateFlags vma_flags = 0,
          vk::ImageLayout init_layout = vk::ImageLayout::eUndefined);

    auto Layout() -> vk::ImageLayout;
    auto Width() -> size_t;
    auto Height() -> size_t;

  private:
    vk::ImageLayout layout;
    size_t height;
    size_t width;
};
// clang-format on

} // namespace nj::ren

#endif
