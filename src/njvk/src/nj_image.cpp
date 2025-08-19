#include "nj_image.h"
#include "nj_allocator.h"
#include "njvklog.h"

namespace nj::ren {

auto Image::HandleName() const noexcept -> std::string { return "Image"; }

Image::Image(vk::Image image) : AllocationUnit({}), shouldDestroy{false} {
    handle = std::make_unique<vk::Image>(image);
}

Image::Image(ren::DeviceH device, ren::AllocatorH allocator, size_t width,
             size_t height, size_t depth, vk::Format format,
             vk::ImageUsageFlags usage, VmaMemoryUsage memory_usage,
             VmaAllocationCreateFlags vma_flags, vk::ImageLayout init_layout)
    : AllocationUnit{allocator}, shouldDestroy{true} {

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
        allocator->Handle(), &static_cast<VkImageCreateInfo &>(image_info),
        &vmaAllocInfo, &image, &allocation, &allocationInfo));

    log::CheckCall(res, "Cant create vulkan image");
    handle = std::make_unique<vk::Image>(image);
}

Image::~Image() {
    log::Debug("Deleting vkImage with id={}...", id);
    if (shouldDestroy) {
        vmaDestroyImage(allocator->Handle(), *handle, allocation);
    }
}

auto Image::Layout() -> vk::ImageLayout & { return layout; }
auto Image::Width() -> size_t { return width; }
auto Image::Height() -> size_t { return height; }

} // namespace nj::ren
