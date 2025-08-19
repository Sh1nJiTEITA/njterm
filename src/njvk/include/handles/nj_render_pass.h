#pragma once
#ifndef NJ_RENDER_PASS_H
#define NJ_RENDER_PASS_H

#include "nj_handle.h"
#include <memory>

namespace nj::ren {

class RenderPass : public VulkanObjectNative<vk::RenderPass> {
  public:
    RenderPass() = default;
    auto HandleName() const noexcept -> std::string override;
};
using RenderPassH = std::shared_ptr<RenderPass>;

}; // namespace nj::ren

#endif
