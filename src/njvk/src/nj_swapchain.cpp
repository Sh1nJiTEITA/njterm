#include "nj_swapchain.h"
#include "nj_builder.h"
#include "njlog.h"
#include "njvkutils.h"
#include <ranges>
#include <vulkan/vulkan_shared.hpp>

// clang-format off
namespace nj::ren {
Swapchain::Swapchain(PhysicalDeviceH phDevice, DeviceH device, SurfaceH surface,
              vk::Extent2D ext, vk::ImageUsageFlags flags)
    : extent{ext} {
    ren::VarHandles h;


    std::vector<vk::SurfaceFormatKHR> surface_formats = phDevice->Handle().getSurfaceFormatsKHR(surface->Handle()); 
    if ( surface_formats.empty() ) { 
        log::FatalExit("Now window (SurfaceKHR) formats available for current device");
    }
    format = ( surface_formats[0].format == vk::Format::eUndefined ) 
               ? vk::Format::eB8G8R8A8Unorm 
               : surface_formats[0].format;

    vk::SurfaceCapabilitiesKHR surface_cap = phDevice->Handle().getSurfaceCapabilitiesKHR(surface->Handle());
    if (surface_cap.currentExtent.width == (std::numeric_limits<uint32_t>::max())) { 
        extent.width = std::clamp( extent.width, surface_cap.minImageExtent.width, surface_cap.maxImageExtent.width );
        extent.height = std::clamp( extent.height, surface_cap.minImageExtent.height, surface_cap.maxImageExtent.height );
    } else { 
        extent = surface_cap.currentExtent;
    }
    
    vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;

    vk::SurfaceTransformFlagBitsKHR pre_transform = ( surface_cap.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity )
                                                    ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                                    : surface_cap.currentTransform;

    vk::CompositeAlphaFlagBitsKHR alpha = build::CompositeAlpha(surface_cap);

    auto info = vk::SwapchainCreateInfoKHR{}
        .setSurface(surface->Handle())
        .setCompositeAlpha(alpha)
        .setPreTransform(pre_transform)
        .setPresentMode(present_mode)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setImageExtent(extent)
        .setImageFormat(format)
        .setMinImageCount(build::PickMinImageCount(surface_cap))
        // TODO: Add support for other color spaces 
        .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
        .setImageArrayLayers(1)
        .setClipped(true)
        .setImageUsage(flags)
    ;
    
    const size_t present_idx = phDevice->PresentQueueIndex();
    const size_t graphics_idx = phDevice->QueueIndex(vk::QueueFlagBits::eGraphics);

    if ( present_idx != graphics_idx  )
    {
        log::Debug("Present queue idx != graphics queue idx");
        // NOTE: Code sample from https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/samples/05_InitSwapchain/05_InitSwapchain.cpp
        // If the graphics and present queues are from different queue families, we either have to explicitly transfer
        // ownership of images between the queues, or we have to create the swapchain with imageSharingMode as
        // VK_SHARING_MODE_CONCURRENT
            
        auto& indices = h.Handle(
            phDevice->UniqueQueueIndices() 
            | std::views::transform([](auto idx) { return static_cast<uint32_t>(idx); })
            | std::ranges::to<std::vector>()
        );
        info.setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(indices);
    }
    
    handle = device->Handle().createSwapchainKHRUnique(info);
}

auto Swapchain::Extent() -> vk::Extent2D { return extent; }
auto Swapchain::ExtentPixels() -> glm::ivec2 { 
    return { extent.width, extent.height };
}
auto Swapchain::Format() -> vk::Format { return format; }

// clang-format off
auto Swapchain::UpdateImages(ren::DeviceH device) -> void {
    assert(images.empty() && "No need to update swaphacin images ...");
    std::vector<vk::Image> new_images = device->Handle().getSwapchainImagesKHR(*handle);
    size_t i = 0;
    for (auto &im : new_images) {
        // auto shared_image = vk::SharedImage(im, device->Handle(), vk::SwapchainOwns::yes);
        auto shared_image = std::make_shared<Image>(im);
        nj::log::Debug("Updating swapchain image #{}", i++);
        images.push_back(std::move(shared_image));
    }
}
// clang-format on

auto Swapchain::Images() -> const std::vector<ImageH>& { return images; }

auto Swapchain::HandleName() const noexcept -> std::string {
    return "Swapchain";
}

} // namespace nj::ren
