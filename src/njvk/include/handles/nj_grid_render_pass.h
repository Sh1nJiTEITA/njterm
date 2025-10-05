#pragma once
#ifndef NJ_GRID_RENDER_PASS_H
#define NJ_GRID_RENDER_PASS_H

#include "nj_attachment_color.h"
#include "nj_buffer.h"
#include "nj_command_buffer.h"
#include "nj_device.h"
#include "nj_pipeline.h"
#include "nj_render_pass.h"
#include <glm/glm.hpp>

namespace nj::ren {

class GridRenderPass : public RenderPass {
public:
    GridRenderPass(DeviceH device, AttachmentColorH colorAtt);
    auto HandleName() const noexcept -> std::string override;

    auto CreateGuidelinesBuffer(
        DeviceH device, AllocatorH allocator, const glm::ivec2& ext,
        const glm::ivec2& face_sz
    ) -> void;
    auto DestroyGuidelinesBuffer() -> void;
    auto RenderGuidelines(CommandBufferH cmd, PipelineH pipeline) -> void;

    auto CreateCellsBuffer(
        DeviceH deviec, AllocatorH allocator, size_t cells,
        const glm::ivec2& ext, const glm::ivec2& face_sz
    ) -> void;
    auto DestroyCellsBuffer() -> void;
    auto RenderCells(CommandBufferH cmd, PipelineH pipeline) -> void;

    auto CreateMonoSpaceGrid() -> void;
    auto DestroyMonoSpaceGrid() -> void;

private:
    size_t guidelinesCount;
    BufferU guidelinesBuffer;
    size_t rows, cols;
    BufferU cellsBuffer;
};

} // namespace nj::ren

#endif
