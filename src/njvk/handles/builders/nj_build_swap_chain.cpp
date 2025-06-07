#include "nj_build_swap_chain.h"
#include "nj_builder.h"
#include "njlog.h"
#include <limits>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {

using BSwapchain = Builder<vk::SwapchainKHR>;

BSwapchain::Builder(vk::SharedPhysicalDevice phDevice, vk::SharedDevice device,
                    vk::SharedSurfaceKHR surface, uint32_t width,
                    uint32_t height)
    : phDevice{phDevice}, device{device}, surface{surface},
      extent{width, height} {}

// clang-format off
BSwapchain::Handle BSwapchain::Build() {
    std::vector<vk::SurfaceFormatKHR> surface_formats = phDevice->getSurfaceFormatsKHR(*surface); 
    if ( surface_formats.empty() ) { 
        log::FatalExit("Now window (SurfaceKHR) formats available for current device");
    }
    vk::Format format = ( surface_formats[0].format == vk::Format::eUndefined ) 
                        ? vk::Format::eB8G8R8A8Unorm 
                        : surface_formats[0].format;

    vk::SurfaceCapabilitiesKHR surface_cap = phDevice->getSurfaceCapabilitiesKHR(*surface);
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

    auto present_idx_opt = PickSurfaceFamilyIndex(phDevice, surface);

    auto info = vk::SwapchainCreateInfoKHR{}
        .setSurface(*surface)
        .setCompositeAlpha(alpha)
        .setPreTransform(pre_transform)
        .setPresentMode(present_mode)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setImageExtent(extent)
        .setImageFormat(format)
        .setMinImageCount(MinImageCount(surface_cap))
        // TODO: Add support for other color spaces 
        .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
        .setImageArrayLayers(1)
        .setClipped(true)
    ;
}

vk::Extent2D BSwapchain::Extent() { 
    return extent;
}

} // namespace nj::build
