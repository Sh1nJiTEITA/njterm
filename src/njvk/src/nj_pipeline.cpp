#include "nj_pipeline.h"
#include "nj_vertex.h"
#include "njlog.h"
#include "njvklog.h"
#include "njvkutils.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off
auto Pipeline::LayoutHandle() -> vk::SharedPipelineLayout { return layout; }

auto Pipeline::HandleName() const noexcept -> std::string {
    return "Pipeline";
}

Pipeline::Pipeline(DeviceH device, RenderPassH render_pass,
                   PipelineBuilderInterfaceH builder,
                   const fs::path &shader_directory) {
    ren::VarHandles h{};
    auto pipeline_info = vk::GraphicsPipelineCreateInfo{}
        .setPVertexInputState( &h.Handle(builder->VertexInputState()) )
        .setPInputAssemblyState( &h.Handle(builder->InputAssemblyState()) )
        .setPDynamicState( &h.Handle(builder->DynamicState()) )
        .setPViewportState( &h.Handle(builder->ViewportState()) )
        .setPRasterizationState( &h.Handle(builder->RasterizationState()) )
        .setPMultisampleState( &h.Handle(builder->MultisampleState()) )
        .setPColorBlendState( &h.Handle(builder->ColorBlendState()) )
        .setRenderPass( render_pass->Handle().get() )
        .setSubpass( 0 )
        .setBasePipelineHandle( VK_NULL_HANDLE )
        .setLayout( *layout )
        .setStages( h.Handle(CreateShaderCreateInfos(device, shader_directory)) )
        ;
    auto result = device->Handle()->createGraphicsPipeline({}, pipeline_info);
    log::CheckCall(result.result, "Creating pipeline failed...");
    handle = vk::SharedPipeline(result.value, device->Handle());
}
// clang-format on

auto ReadShaderFile(const fs::path &path) -> std::vector<char> {

    if (!fs::exists(path)) {
        log::FatalExit("Shader file with path={} does not exist",
                       path.string());
    }

    if (!fs::is_regular_file(path)) {
        log::FatalExit(
            "Input path={} for shader file leads not to regular file",
            path.string());
    }

    std::ifstream file(path, std::ios::ate | std::ios::binary);
    std::vector<char> data{};

    size_t file_size = (size_t)file.tellg();
    data.resize(file_size);
    file.seekg(0);
    file.read(data.data(), file_size);
    file.close();
    return data;
}

// clang-format off
auto CreateShaderModule(ren::DeviceH device, const std::vector<char> &data)
    -> vk::SharedShaderModule {
    
    auto info = vk::ShaderModuleCreateInfo{}
        .setPCode( reinterpret_cast<const uint32_t*>(data.data()) )
        .setCodeSize( data.size() );

    auto shader_module = device->Handle()->createShaderModule(info);
    return vk::SharedShaderModule(shader_module, device->Handle());
}


auto CreateShaderModule(ren::DeviceH device, const fs::path& path) -> vk::SharedShaderModule { 
    auto data = ReadShaderFile(path);
    return CreateShaderModule(device, data);
}

auto CreateShaderCreateInfos(ren::DeviceH device, const fs::path &path) -> std::vector<vk::PipelineShaderStageCreateInfo> {

    static std::unordered_map<std::string, vk::ShaderStageFlagBits> stage_bits = {
        {"vert", vk::ShaderStageFlagBits::eVertex},
        {"frag", vk::ShaderStageFlagBits::eFragment},
        {"geom", vk::ShaderStageFlagBits::eGeometry},
        {"comp", vk::ShaderStageFlagBits::eCompute},
        {"tesse", vk::ShaderStageFlagBits::eTessellationEvaluation},
        {"tessc", vk::ShaderStageFlagBits::eTessellationControl}
    };

    std::vector<fs::path> filenames;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        auto path = entry.path();
        if (path.extension() == ".spv") {
            filenames.push_back(std::move(path));
        } 
    }

    std::vector<vk::PipelineShaderStageCreateInfo> infos;
    infos.reserve(filenames.size());

    for (size_t i = 0; i < filenames.size(); ++i) {
        vk::SharedShaderModule module = CreateShaderModule(device, filenames[i]);
        auto& info = *infos.insert(infos.end(), {});
        info.setPName("main")
            .setModule(module.get())
            .setStage(stage_bits[filenames[i].stem()]);
    }
    return infos;
}




auto PipelineBuilderTest::VertexInputState() -> vk::PipelineVertexInputStateCreateInfo {
    auto& binding_description = h.Handle( Vertex::BindingDescription() );
    auto& attribute_descriptions = h.Handle( Vertex::AttributeDescriptions() );

    return vk::PipelineVertexInputStateCreateInfo{}
        .setVertexBindingDescriptions( binding_description )
        .setVertexAttributeDescriptions( attribute_descriptions )
    ;
}
auto PipelineBuilderTest::InputAssemblyState()  -> vk::PipelineInputAssemblyStateCreateInfo {
    return vk::PipelineInputAssemblyStateCreateInfo{}
        .setTopology( vk::PrimitiveTopology::eTriangleList )
        .setPrimitiveRestartEnable( false )
    ;
    
}
auto PipelineBuilderTest::DynamicState()  -> vk::PipelineDynamicStateCreateInfo {
    auto& states = h.Handle( std::array { 
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    } );

    return vk::PipelineDynamicStateCreateInfo{}
        .setDynamicStates(states)
    ;
}
auto PipelineBuilderTest::ViewportState() -> vk::PipelineViewportStateCreateInfo {

    return vk::PipelineViewportStateCreateInfo{}
        .setViewportCount(1)
        .setScissorCount(1)
    ;

}
auto PipelineBuilderTest::RasterizationState() -> vk::PipelineRasterizationStateCreateInfo {
    return vk::PipelineRasterizationStateCreateInfo{}
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
    ;
}
auto PipelineBuilderTest::MultisampleState() -> vk::PipelineMultisampleStateCreateInfo {
    return vk::PipelineMultisampleStateCreateInfo{}
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(false)
        .setAlphaToOneEnable(false)
    ;

}
auto PipelineBuilderTest::ColorBlendState()  -> vk::PipelineColorBlendStateCreateInfo {
    auto& color_att = h.Handle(
        vk::PipelineColorBlendAttachmentState{}
            .setColorWriteMask(vk::ColorComponentFlagBits::eA | 
                               vk::ColorComponentFlagBits::eB | 
                               vk::ColorComponentFlagBits::eG | 
                               vk::ColorComponentFlagBits::eR)
            .setBlendEnable(true)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
    );
    return vk::PipelineColorBlendStateCreateInfo{}
        .setAttachments(color_att)
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)
        .setBlendConstants({ 0.f, 0.f, 0.f, 0.f})
    ;
}

auto PipelineBuilderTest::PipelineLayoutInfo() -> vk::PipelineLayoutCreateInfo {

}

// clang-format on

} // namespace nj::ren
