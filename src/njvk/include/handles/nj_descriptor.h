#pragma once
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_allocator.h"
#include "nj_buffer.h"
#include "nj_device.h"
#include "nj_handle.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class DescriptorPool : public VulkanObject<vk::DescriptorPool> {
  public:
    DescriptorPool(ren::DeviceH device);
    auto HandleName() const noexcept -> std::string override;
};
using DescriptorPoolH = std::shared_ptr<DescriptorPool>;

// clang-format off
struct Descriptor {
    //! Main descriptor creation
    //! @param layout Shader layout number to connect to
    //! @param binding Shader binding number to connect to
    //! @param stages Shader stages to work in
    //! @param type Vulkan descriptor type enum value
    Descriptor(size_t layout, size_t binding, vk::DescriptorType type,
               vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex |
                                             vk::ShaderStageFlagBits::eFragment)
        : layout{layout}, binding{binding}, shaderStages{stages}, type{type} {}

    virtual ~Descriptor() { }
    Descriptor(const Descriptor &) = delete;
    Descriptor &operator=(const Descriptor &) = delete;

    virtual void CreateBuffer(ren::DeviceH device, ren::AllocatorH allocator) = 0;
    virtual void CreateImage(ren::DeviceH device, ren::AllocatorH allocator) = 0;
    virtual void CreateView(ren::DeviceH device, ren::AllocatorH allocator) = 0;

    auto LayoutBinding() -> vk::DescriptorSetLayoutBinding;
    virtual auto BufferInfo() -> vk::DescriptorBufferInfo;
    virtual auto ImageInfo() -> vk::DescriptorImageInfo;

    auto Layout() const noexcept -> size_t; 
    auto Binding() const noexcept -> size_t; 
    auto ShaderStages() const noexcept -> vk::ShaderStageFlags; 
    auto DescriptorType() const noexcept -> vk::DescriptorType; 
    auto HasBuffer() const noexcept -> bool;
    auto HasImage() const noexcept -> bool;

  protected:
    const size_t layout;
    const size_t binding;
    const vk::ShaderStageFlags shaderStages;
    const vk::DescriptorType type;

    std::unique_ptr<Buffer> buffer;
    std::unique_ptr<Image> image;
    std::unique_ptr<vk::SharedImageView> imageView;
};
// clang-format on

} // namespace nj::ren

#endif
