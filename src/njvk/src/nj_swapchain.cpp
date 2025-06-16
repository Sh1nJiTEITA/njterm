#include "nj_swapchain.h"
#include "nj_build_swapchain.h"
#include "njlog.h"
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {
Swapchain::Swapchain(ren::PhysicalDeviceH phDevice, ren::DeviceH device,
                     vk::SharedSurfaceKHR surface, vk::Extent2D ext)
    : extent{ext} {
    auto builder = build::Builder<vk::SwapchainKHR>(phDevice, device, surface,
                                                    ext.width, ext.height);
    handle = builder.Build();
    format = builder.Format();
}

auto Swapchain::Extent() -> vk::Extent2D { return extent; }
auto Swapchain::Format() -> vk::Format { return format; }

// clang-format off
auto Swapchain::UpdateImages(ren::DeviceH device) -> void {
    assert(images.empty() && "No need to update swaphacin images ...");
    std::vector<vk::Image> new_images = device->Handle()->getSwapchainImagesKHR(*handle);
    size_t i = 0;
    for (auto &im : new_images) {
        auto shared_image = vk::SharedImage(im, device->Handle(), vk::SwapchainOwns::yes);
        nj::log::Debug("Updating swapchain image #{}", i++);
        images.push_back(std::move(shared_image));
    }
}
// clang-format on

auto Swapchain::Images() -> const std::vector<vk::SharedImage> & {
    return images;
}

} // namespace nj::ren
