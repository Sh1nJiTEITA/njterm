#pragma once
#ifndef NJ_GRID_RENDER_PASS_H
#define NJ_GRID_RENDER_PASS_H

#include "nj_attachment_color.h"
#include "nj_device.h"
#include "nj_render_pass.h"

namespace nj::ren {

class GridRenderPass : public RenderPass {
  public:
    GridRenderPass(DeviceH device, AttachmentColor colorAtt);
};

} // namespace nj::ren

#endif
