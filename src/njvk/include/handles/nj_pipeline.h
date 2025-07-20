#pragma once
#ifndef NJ_PIPELINE_H
#define NJ_PIPELINE_H

#include "nj_handle.h"
#include "njvkutils.h"
#include <filesystem>
#include <nj_device.h>
#include <nj_render_pass.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace fs = std::filesystem;
namespace nj::ren {

// clang-format off
class PipelineBuilderInterface {
  public:
    virtual auto VertexInputState() -> vk::PipelineVertexInputStateCreateInfo = 0;
    virtual auto InputAssemblyState()-> vk::PipelineInputAssemblyStateCreateInfo = 0;
    virtual auto DynamicState() -> vk::PipelineDynamicStateCreateInfo = 0;
    virtual auto ViewportState() -> vk::PipelineViewportStateCreateInfo = 0;
    virtual auto RasterizationState() -> vk::PipelineRasterizationStateCreateInfo = 0;
    virtual auto MultisampleState() -> vk::PipelineMultisampleStateCreateInfo = 0;
    virtual auto ColorBlendState() -> vk::PipelineColorBlendStateCreateInfo = 0;
    virtual auto PipelineLayoutInfo() -> vk::PipelineLayoutCreateInfo = 0;
};
using PipelineBuilderInterfaceH = std::shared_ptr< PipelineBuilderInterface >;

// clang-format on
class Pipeline : ren::VulkanObject<vk::Pipeline> {
  public:
    Pipeline(DeviceH device, RenderPassH render_pass,
             PipelineBuilderInterfaceH builder,
             const fs::path &shader_directory);

    auto LayoutHandle() -> vk::SharedPipelineLayout;
    auto HandleName() const noexcept -> std::string override;

  private:
    vk::SharedPipelineLayout layout;
};
// clang-format off

auto ReadShaderFile(const fs::path& path) -> std::vector<char>;
auto CreateShaderModule(ren::DeviceH device, const std::vector<char>& data) -> vk::SharedShaderModule;
auto CreateShaderModule(ren::DeviceH device, const fs::path& path) -> vk::SharedShaderModule;
auto CreateShaderCreateInfos(ren::DeviceH device, const fs::path& path) -> std::vector< vk::PipelineShaderStageCreateInfo >;

class PipelineBuilderTest : public PipelineBuilderInterface {
  public:
    virtual auto VertexInputState() -> vk::PipelineVertexInputStateCreateInfo override;
    virtual auto InputAssemblyState() -> vk::PipelineInputAssemblyStateCreateInfo override;
    virtual auto DynamicState() -> vk::PipelineDynamicStateCreateInfo override;
    virtual auto ViewportState() -> vk::PipelineViewportStateCreateInfo override;
    virtual auto RasterizationState() -> vk::PipelineRasterizationStateCreateInfo override;
    virtual auto MultisampleState() -> vk::PipelineMultisampleStateCreateInfo override;
    virtual auto ColorBlendState() -> vk::PipelineColorBlendStateCreateInfo override;

    // TODO: IMPLEMENT!
    virtual auto PipelineLayoutInfo() -> vk::PipelineLayoutCreateInfo override;
  private:
    ren::VarHandles h;
};
                             

}; // namespace nj::ren

#endif
