#include "nj_attachment_color.h"
#include "njlog.h"

namespace nj::ren {

AttachmentColor::AttachmentColor(ren::SwapchainH swapchain)
    : Attachment{Attachment::Type::Color}, swapchainWeak{swapchain} {}

auto AttachmentColor::Description() -> vk::AttachmentDescription {
    auto sc = swapchainWeak.lock();
    if (!sc) {
        log::FatalExit("Cant lock swapchain pointer "
                       "during creation of "
                       "image color attachment (::Description)");
    }

    return vk::AttachmentDescription{}
        .setFormat(sc->Format())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
}
auto AttachmentColor::Reference(uint32_t idx) -> vk::AttachmentReference {
    return vk::AttachmentReference{}
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setAttachment(idx);
}
auto AttachmentColor::Dependency() -> vk::SubpassDependency {
    return vk::SubpassDependency{}
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
}

auto AttachmentColor::CreateData(uint32_t frame_idx, DeviceH device)
    -> AttachmentData {
    auto sc = swapchainWeak.lock();
    if (!sc) {
        log::FatalExit("Cant lock swapchain pointer "
                       "during creation of "
                       "image color attachment");
    }
    vk::SharedImage image = sc->Images().at(frame_idx);

    const auto com = vk::ComponentMapping{
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
    };

    const auto sub = vk::ImageSubresourceRange{
        /* aspectMask     */ vk::ImageAspectFlagBits::eColor,
        /* baseMipLevel   */ 0,
        /* levelCount     */ 1,
        /* baseArrayLayer */ 0,
        /* layerCount     */ 1,
    };

    const auto view_info = vk::ImageViewCreateInfo{}
                               .setImage(*image)
                               .setFormat(sc->Format())
                               .setViewType(vk::ImageViewType::e2D)
                               .setComponents(com)
                               .setSubresourceRange(sub);
    vk::ImageView raw_view = device->Handle()->createImageView(view_info);
    vk::SharedImageView view = vk::SharedImageView(raw_view, device->Handle());
    return AttachmentData(shared_from_this(), image, view);
}

} // namespace nj::ren
