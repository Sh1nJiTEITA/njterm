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
    virtual auto PipelineLayout(DeviceH device, const std::vector<vk::DescriptorSetLayout>& layouts) -> vk::UniquePipelineLayout = 0;
};
using PipelineBuilderInterfaceH = std::shared_ptr< PipelineBuilderInterface >;

// clang-format on
class Pipeline : public ren::VulkanObjectNative<vk::Pipeline> {
public:
    Pipeline(
        DeviceH device, RenderPassH render_pass,
        PipelineBuilderInterfaceH builder,
        const std::vector<vk::DescriptorSetLayout>& layouts,
        const fs::path& shader_directory
    );
    virtual ~Pipeline();

    auto LayoutHandle() -> vk::PipelineLayout;
    auto HandleName() const noexcept -> std::string override;

private:
    vk::UniquePipelineLayout layout;
};
using PipelineH = std::shared_ptr<Pipeline>;

// clang-format off

auto ReadShaderFile(const fs::path& path) -> std::vector<char>;
auto CreateShaderModule(ren::DeviceH device, const std::vector<char>& data) -> vk::UniqueShaderModule;
auto CreateShaderModule(ren::DeviceH device, const fs::path& path) -> vk::UniqueShaderModule;
auto CreateShaderCreateInfos(ren::DeviceH device, const fs::path &path, std::vector< vk::UniqueShaderModule >& modules) -> std::vector< vk::PipelineShaderStageCreateInfo >;

class PipelineBuilderBase : public PipelineBuilderInterface {
  public:
    virtual auto VertexInputState() -> vk::PipelineVertexInputStateCreateInfo override;
    virtual auto InputAssemblyState() -> vk::PipelineInputAssemblyStateCreateInfo override;
    virtual auto DynamicState() -> vk::PipelineDynamicStateCreateInfo override;
    virtual auto ViewportState() -> vk::PipelineViewportStateCreateInfo override;
    virtual auto RasterizationState() -> vk::PipelineRasterizationStateCreateInfo override;
    virtual auto MultisampleState() -> vk::PipelineMultisampleStateCreateInfo override;
    virtual auto ColorBlendState() -> vk::PipelineColorBlendStateCreateInfo override;
    virtual auto PipelineLayout(DeviceH device, const std::vector<vk::DescriptorSetLayout>& layouts) -> vk::UniquePipelineLayout override;
  private:
    ren::VarHandles h;
};
                             

}; // namespace nj::ren

#endif
