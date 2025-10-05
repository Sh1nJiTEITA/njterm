#include "nj_grid_render_pass.h"
#include "nj_allocator.h"
#include "nj_vertex.h"
#include "njlog.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

GridRenderPass::GridRenderPass(DeviceH device, AttachmentColorH colorAtt) {
    vk::AttachmentDescription cdesc = colorAtt->Description();
    vk::AttachmentReference cref = colorAtt->Reference(0);
    vk::SubpassDependency cdep = colorAtt->Dependency();
    auto csub_desc = vk::SubpassDescription{}
                         .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                         .setPColorAttachments(&cref)
                         .setColorAttachmentCount(1);

    auto info = vk::RenderPassCreateInfo{}
                    .setSubpassCount(1)
                    .setPSubpasses(&csub_desc)

                    .setAttachmentCount(1)
                    .setPAttachments(&cdesc)

                    .setDependencyCount(1)
                    .setPDependencies(&cdep);

    handle = device->Handle().createRenderPassUnique(info);
}

auto GridRenderPass::HandleName() const noexcept -> std::string {
    return "Grid RenderPass";
}

auto GridRenderPass::CreateGuidelinesBuffer(
    DeviceH device, AllocatorH allocator, const glm::ivec2& ext,
    const glm::ivec2& face_sz
) -> void {
    log::Debug("Creating guidelines buffer for extent: ({}, {})", ext.x, ext.y);
    log::Debug(
        "Creating guidelines buffer for face size: ({}, {})", face_sz.x,
        face_sz.y
    );
    const size_t face_height = face_sz.y;
    guidelinesCount = ext.y / face_height;
    log::Debug(
        "guidelinesCount = {} / {} = {}", ext.y, face_height, guidelinesCount
    );
    const glm::ivec2 begin_pos{0, face_height};
    const size_t vertices_count = guidelinesCount * 2;
    const size_t allocation_sz = vertices_count * sizeof(Vertex);
    guidelinesBuffer = std::make_unique<Buffer>(
        device, allocator, allocation_sz,
        vk::BufferUsageFlagBits::eVertexBuffer,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT
    );
    void* raw_data = guidelinesBuffer->Map();
    Vertex* data = static_cast<Vertex*>(raw_data);
    for (size_t line_idx = 0; line_idx < guidelinesCount; ++line_idx) {
        const size_t l_idx = line_idx * 2;
        const size_t r_idx = l_idx + 1;
        const int height = face_height * (1 + line_idx);
        const float relative_height = static_cast<float>(height) / ext.y;
        const float norm_height = 2.0f * relative_height - 1.0f;

        data[l_idx] = Vertex{.pos = {-1, norm_height}};
        data[r_idx] = Vertex{.pos = {+1, norm_height}};

        log::Debug(
            "Guideline {} : HEIGHT={} LEFT=( x={}, y={} ) RIGHT=( x={}, y={} )",
            line_idx, height, data[l_idx].pos.x, data[l_idx].pos.y,
            data[r_idx].pos.x, data[r_idx].pos.y
        );
    }
    guidelinesBuffer->Unmap();
}

auto GridRenderPass::DestroyGuidelinesBuffer() -> void {
    guidelinesBuffer.reset();
}

auto GridRenderPass::RenderGuidelines(CommandBufferH cmd, PipelineH pipeline)
    -> void {
    cmd->Handle().bindVertexBuffers(0, {guidelinesBuffer->Handle()}, {0});
    cmd->Handle().bindPipeline(
        vk::PipelineBindPoint::eGraphics, pipeline->Handle()
    );
    // log::Debug("Guidelines count={}", guidelinesCount);
    cmd->Handle().draw(guidelinesCount * 2, 1, 0, 0);
}

auto CreateMonoSpaceGrid() -> void {}

auto DestroyMonoSpaceGrid() -> void {}

// clang-format on

} // namespace nj::ren
