#pragma once
#ifndef NJ_RENDER_PASS_H
#define NJ_RENDER_PASS_H

#include "nj_handle.h"

namespace nj::ren {

class RenderPass : public VulkanObject<vk::RenderPass> {
  public:
    RenderPass() = default;
};

}; // namespace nj::ren

#endif
