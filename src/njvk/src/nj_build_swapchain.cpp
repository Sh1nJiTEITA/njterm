#include "nj_build_swapchain.h"
#include "nj_builder.h"
#include "njlog.h"
#include <limits>
#include <ranges>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {

using BSwapchain = Builder<vk::SwapchainKHR>;

BSwapchain::Builder(ren::PhysicalDeviceH phDevice, ren::DeviceH device,
                    vk::SharedSurfaceKHR surface, uint32_t width,
                    uint32_t height)
    : phDevice{phDevice}, device{device}, surface{surface},
      extent{width, height} {}

// clang-format off
BSwapchain::Handle BSwapchain::Build() {
    std::vector<vk::SurfaceFormatKHR> surface_formats = phDevice->Handle()->getSurfaceFormatsKHR(*surface); 
    if ( surface_formats.empty() ) { 
        log::FatalExit("Now window (SurfaceKHR) formats available for current device");
    }
    vk::Format format = ( surface_formats[0].format == vk::Format::eUndefined ) 
                        ? vk::Format::eB8G8R8A8Unorm 
                        : surface_formats[0].format;

    vk::SurfaceCapabilitiesKHR surface_cap = phDevice->Handle()->getSurfaceCapabilitiesKHR(*surface);
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

    vk::CompositeAlphaFlagBitsKHR alpha = CompositeAlpha(surface_cap);


    auto info = vk::SwapchainCreateInfoKHR{}
        .setSurface(*surface)
        .setCompositeAlpha(alpha)
        .setPreTransform(pre_transform)
        .setPresentMode(present_mode)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setImageExtent(extent)
        .setImageFormat(format)
        .setMinImageCount(PickMinImageCount(surface_cap))
        // TODO: Add support for other color spaces 
        .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
        .setImageArrayLayers(1)
        .setClipped(true)
    ;
    
    const size_t present_idx = phDevice->PresentQueueIndex();
    const size_t graphics_idx = phDevice->QueueIndex(vk::QueueFlagBits::eGraphics);

    if ( present_idx != graphics_idx  )
    {
        // NOTE: Code sample from https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/samples/05_InitSwapchain/05_InitSwapchain.cpp
        // If the graphics and present queues are from different queue families, we either have to explicitly transfer
        // ownership of images between the queues, or we have to create the swapchain with imageSharingMode as
        // VK_SHARING_MODE_CONCURRENT
            
        auto& indices = h.Handle(
            phDevice->UniqueQueueIndices() | std::views::transform([](auto idx) { return static_cast<uint32_t>(idx); })
                                           | std::ranges::to<std::vector>()
        );
        info.setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(indices);
    }
    
    vk::SwapchainKHR swapchain_raw = device->Handle()->createSwapchainKHR(info);
    return vk::SharedSwapchainKHR(swapchain_raw, device->Handle(), surface);
}

vk::Extent2D BSwapchain::Extent() { 
    return extent;
}

} // namespace nj::build
