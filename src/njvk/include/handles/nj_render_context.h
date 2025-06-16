#pragma once
#ifndef NJ_RENDER_CONTEXT_H
#define NJ_RENDER_CONTEXT_H

#include "nj_attachment.h"
#include "nj_builder.h"
#include "nj_device.h"
#include "nj_swapchain.h"
#include <memory>
#include <vector>

namespace nj::ren {

class FrameData {
  public:
    FrameData() = default;
    FrameData(ren::DeviceH device, ren::SwapchainH swapchain,
              const std::vector<ren::AttachmentDataH> &att);

    auto Framebuffer() -> vk::SharedFramebuffer &;
    auto Attachement(size_t idx) -> const ren::AttachmentDataH &;

  private:
    vk::SharedFramebuffer framebuffer;
    std::vector<ren::AttachmentDataH> attachments;
};
using FrameDataH = std::shared_ptr<FrameData>;

class RenderContext {

  public:
    RenderContext(ren::DeviceH device, ren::SwapchainH swapchain,
                  const std::vector<ren::AttachmentH> &att);

  protected:
  private:
    std::vector<FrameDataH> frameDatas;
};
using RenderContextH = std::shared_ptr<RenderContext>;

}; // namespace nj::ren

#endif
