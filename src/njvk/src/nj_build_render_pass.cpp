#include "nj_build_render_pass.h"

namespace nj::build {

using BRenderPass = Builder<vk::RenderPass>;

BRenderPass::Builder(ren::DeviceH device,
                     const std::vector<ren::AttachmentH> &att)
    : device{device} {}

BRenderPass::Handle BRenderPass::Build() {}

} // namespace nj::build
