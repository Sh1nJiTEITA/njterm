#include "nj_pipeline_guidelines.h"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off

auto PipelineBuilderGuidelines::InputAssemblyState()  -> vk::PipelineInputAssemblyStateCreateInfo {
    return vk::PipelineInputAssemblyStateCreateInfo{}
        .setTopology( vk::PrimitiveTopology::eLineList )
        .setPrimitiveRestartEnable( false )
    ;
    
}

auto PipelineBuilderGuidelines::RasterizationState() -> vk::PipelineRasterizationStateCreateInfo {
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
