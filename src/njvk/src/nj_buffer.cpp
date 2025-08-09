#include "nj_buffer.h"
#include "njlog.h"
#include "njvklog.h"
#include "njvkutils.h"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

AllocationUnit::AllocationUnit(ren::AllocatorH allocator)
    : allocator{allocator}, id{GenerateId()} {}
auto AllocationUnit::Allocation() -> VmaAllocation & { return allocation; }
auto AllocationUnit::Size() -> size_t { return allocationInfo.size; }
auto AllocationUnit::Id() -> uint32_t { return id; }

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

    handle = std::make_shared< vk::Buffer >();

    VkBuffer pBuffer {};

    auto res = static_cast<vk::Result>(vmaCreateBuffer(
        *allocator->CHandle(),
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
    vmaDestroyBuffer(*allocator->CHandle(), *handle, allocation);
}

// clang-format on

auto Buffer::Map() -> void * {
    void *p;
    vmaMapMemory(*allocator->CHandle(), allocation, &p);
    return p;
}

auto Buffer::Unmap() -> void {
    vmaUnmapMemory(*allocator->CHandle(), allocation);
}

auto Buffer::HandleName() const noexcept -> std::string { return "Buffer"; }

Image::Image(ren::DeviceH device, ren::AllocatorH allocator, size_t width,
             size_t height, size_t depth, vk::Format format,
             vk::ImageUsageFlags usage, VmaMemoryUsage memory_usage,
             VmaAllocationCreateFlags vma_flags, vk::ImageLayout init_layout)
    : AllocationUnit{allocator} {

    vk::Extent3D ext{static_cast<uint32_t>(width),
                     static_cast<uint32_t>(height),
                     static_cast<uint32_t>(depth)};

    // FIXME: Find out if image type of (2D or 3D) is related to
    // existance of depth etc...
    auto image_info = vk::ImageCreateInfo{}
                          .setImageType(vk::ImageType::e2D)
                          .setFormat(format)
                          .setExtent(ext)
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setSamples(vk::SampleCountFlagBits::e1)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setUsage(usage)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setInitialLayout(init_layout);

    VmaAllocationCreateInfo vmaAllocInfo{
        .flags = vma_flags,
        .usage = memory_usage,
    };

    VkImage image{};
    auto res = static_cast<vk::Result>(vmaCreateImage(
        *allocator->CHandle(), &static_cast<VkImageCreateInfo &>(image_info),
        &vmaAllocInfo, &image, &allocation, &allocationInfo));

    log::CheckCall(res, "Cant create vulkan image");

    handle = vk::SharedImage(image, device->Handle(), {});
}

auto Image::Layout() -> vk::ImageLayout { return layout; }
auto Image::Width() -> size_t { return width; }
auto Image::Height() -> size_t { return height; }
auto Image::HandleName() const noexcept -> std::string { return "Image"; }

} // namespace nj::ren
