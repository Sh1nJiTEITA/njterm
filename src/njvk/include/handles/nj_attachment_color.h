#pragma once
#ifndef NJ_ATTACHMENT_COLOR_H
#define NJ_ATTACHMENT_COLOR_H

#include "nj_attachment.h"
#include "nj_swapchain.h"

namespace nj::ren {

// clang-format off
class AttachmentColor : public Attachment {
  public:
    AttachmentColor(ren::SwapchainH swapchain);
    auto Description() -> vk::AttachmentDescription override;
    auto Reference(uint32_t idx) -> vk::AttachmentReference override;
    auto Dependency() -> vk::SubpassDependency override;
    auto CreateData(uint32_t frame_idx, DeviceH device) -> AttachmentData override;

  protected:
    std::weak_ptr<ren::Swapchain> swapchainWeak;
};
// clang-format on

} // namespace nj::ren

#endif
