#include "nj_render_context.h"

namespace nj::ren {

vk::SharedImage &FrameData::Image() { return image; }
vk::SharedImageView &FrameData::View() { return view; }
vk::SharedFramebuffer &FrameData::Framebuffer() { return framebuffer; }
std::vector<vk::SharedImageView> &FrameData::Attachments() {
    return attachments;
}

} // namespace nj::ren
