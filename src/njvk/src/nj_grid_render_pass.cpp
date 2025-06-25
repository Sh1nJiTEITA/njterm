#include "nj_grid_render_pass.h"

namespace nj::ren {

// clang-format off
GridRenderPass::GridRenderPass(DeviceH device, AttachmentColorH colorAtt) {
    vk::AttachmentDescription cdesc = colorAtt->Description();
    vk::AttachmentReference cref = colorAtt->Reference(0);
    vk::SubpassDependency cdep = colorAtt->Dependency();
    auto csub_desc = vk::SubpassDescription{}
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setPColorAttachments(&cref)
        .setColorAttachmentCount(1)
    ;
    
    auto info = vk::RenderPassCreateInfo{}
        .setSubpassCount(1)
        .setPSubpasses(&csub_desc)

        .setAttachmentCount(1)
        .setPAttachments(&cdesc)

        .setDependencyCount(1)
        .setPDependencies(&cdep)
    ;

    vk::RenderPass raw = device->Handle()->createRenderPass(info); 
    handle = vk::SharedRenderPass(raw, device->Handle());
}

auto GridRenderPass::HandleName() const noexcept -> std::string { 
    return "Grid RenderPass";
}
// clang-format on

} // namespace nj::ren
