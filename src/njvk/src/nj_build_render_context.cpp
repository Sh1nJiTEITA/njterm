#include "nj_build_render_context.h"
#include "nj_render_context.h"
#include "njlog.h"
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {

// using SBFrameData = Builder<ren::FrameData>;
//
// SBFrameData::Builder(ren::DeviceH device, vk::SharedImage swapchain_image,
//                      vk::Format format, vk::Extent2D extent,
//                      const std::vector<vk::SharedImageView> &att)
//     : device{device}, image{swapchain_image}, format{format},
//     attachments{att},
//       extent{extent} {}
//
// // clang-format off
// SBFrameData::Handle SBFrameData::Build() {
//     data = std::make_shared<ren::FrameData>();
//     data->image = BuildImage();
//     data->view = BuildView();
//     data->framebuffer = BuildFramebuffer();
//     return data;
// }
//
//
// auto SBFrameData::BuildImage() -> vk::SharedImage {
//     return image;
// }
//
// auto SBFrameData::BuildView() -> vk::SharedImageView{
//     const auto com = vk::ComponentMapping{
//         vk::ComponentSwizzle::eIdentity,
//         vk::ComponentSwizzle::eIdentity,
//         vk::ComponentSwizzle::eIdentity,
//         vk::ComponentSwizzle::eIdentity,
//     };
//
//     const auto sub = vk::ImageSubresourceRange{
//       /* aspectMask     */   vk::ImageAspectFlagBits::eColor,
//       /* baseMipLevel   */   0,
//       /* levelCount     */   1,
//       /* baseArrayLayer */   0,
//       /* layerCount     */   1,
//     };
//
//     const auto view_info = vk::ImageViewCreateInfo{}
//         .setImage(*data->Image())
//         .setFormat(format)
//         .setViewType(vk::ImageViewType::e2D)
//         .setComponents(com)
//         .setSubresourceRange(sub)
//     ;
//     vk::ImageView view = device->Handle()->createImageView( view_info );
//     return vk::SharedImageView( view, device->Handle() );
// }
//

// using BFramebuffer = Builder<vk::Framebuffer>;
//
// BFramebuffer::Builder(ren::DeviceH device, ren::SwapchainH swapchain,
//                       const std::vector<vk::SharedImageView> &views)
//     : views{views}, device{device}, swapchain{swapchain}
//
// {}
//
// auto BFramebuffer::Build() -> vk::SharedFramebuffer {
//     std::vector<vk::ImageView> transformed_views;
//     for (auto &view : views) {
//         transformed_views.push_back(*view);
//     }
//     vk::Extent2D extent = swapchain->Extent();
//     auto info = vk::FramebufferCreateInfo{}
//                     .setAttachments(transformed_views)
//                     .setWidth(extent.width)
//                     .setHeight(extent.height)
//                     .setLayers(1);
//     vk::Framebuffer frame_buffer = device->Handle()->createFramebuffer(info);
//     return vk::SharedFramebuffer(frame_buffer, device->Handle());
// }
//
// clang-format on
using SBRenderContext = Builder<ren::RenderContext>;

SBRenderContext::Builder(ren::DeviceH device, ren::SwapchainH swapchain,
                         const std::vector<vk::SharedImageView> &att)
    : device{device}, swapchain{swapchain}, attachments{att} {}

SBRenderContext::Builder(ren::DeviceH device, ren::SwapchainH swapchain)
    : device{device}, swapchain{swapchain} {}

// clang-format off
SBRenderContext::Handle SBRenderContext ::Build() {
    // auto context = std::make_shared<ren::RenderContext>();
    // log::Debug("Creating render context...");
    // size_t i = 0;
    // for (auto image : swapchain->Images()) {
    //     auto data = build::Build<ren::FrameData>(
    //         device, 
    //         image, 
    //         swapchain->Format(), 
    //         swapchain->Extent(), 
    //         attachments
    //     );
    //     log::Debug("Created image #{}", i++);
    //     context->frameDatas.push_back(std::move(data));
    // }
    // return context;
}
// clang-format on
}; // namespace nj::build
