#include "nj_pipeline_cells.h"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off

auto PipelineBuilderCells::InputAssemblyState()  -> vk::PipelineInputAssemblyStateCreateInfo {
    return vk::PipelineInputAssemblyStateCreateInfo{}
        .setTopology( vk::PrimitiveTopology::eTriangleList )
        .setPrimitiveRestartEnable( false )
    ;
    
}

auto PipelineBuilderCells::RasterizationState() -> vk::PipelineRasterizationStateCreateInfo {
    return vk::PipelineRasterizationStateCreateInfo{}
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setLineWidth(1.) // NOTE: MUST BE PRESENT!
        ;
}

// clang-format on

} // namespace nj::ren
