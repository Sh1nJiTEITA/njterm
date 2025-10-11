#pragma once
#ifndef NJ_VULKAN_IMAGE_H
#define NJ_VULKAN_IMAGE_H

#include "nj_allocator.h"
#include "nj_device.h"
#include "nj_handle.h"

namespace nj::ren {

// class Image : public VulkanObjectManual<vk::Image> {
//   public:
//     Image(vk::Image image);
//     Image(vk::Image image, DeviceH device);
//     ~Image();
//     auto HandleName() const noexcept -> std::string override;
//
//   protected:
//     const bool shouldDelete;
//     std::weak_ptr<Device> weakDevice;
// };

// clang-format off
class Image : public AllocationUnit, public VulkanObjectManual<vk::Image> {
  public:
    Image(vk::Image image);

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
    ~Image();
    auto Layout() -> vk::ImageLayout&;
    auto Width() -> size_t;
    auto Height() -> size_t;

    auto HandleName() const noexcept -> std::string override;

  protected:
    const bool shouldDestroy;
    vk::ImageLayout layout;
    size_t height;
    size_t width;
    // std::shared_ptr<vk::Image> handle;
};
// clang-format on
using ImageH = std::shared_ptr<Image>;
using ImageU = std::unique_ptr<Image>;

} // namespace nj::ren

#endif
