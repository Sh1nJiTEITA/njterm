#pragma once
#ifndef NJ_BUILD_RENDER_PASS_H
#define NJ_BUILD_RENDER_PASS_H

#include "nj_attachment.h"
#include "nj_builder.h"

namespace nj::build {

template <> class Builder<vk::RenderPass> {
  public:
    Builder(ren::DeviceH device, const std::vector<ren::AttachmentH> &att);

    using Handle = vk::SharedDevice;
    Handle Build();

  private:
    ren::DeviceH device;
    ren::VarHandles h;
};

} // namespace nj::build

#endif
