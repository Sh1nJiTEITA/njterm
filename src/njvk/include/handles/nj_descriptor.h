#pragma once
#include "nj_sampler.h"
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_allocator.h"
#include "nj_buffer.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "nj_image.h"
#include "nj_image_view.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

class DescriptorPool : public VulkanObjectNative<vk::DescriptorPool> {
public:
    DescriptorPool(ren::DeviceH device);
    auto HandleName() const noexcept -> std::string override;
};
using DescriptorPoolH = std::shared_ptr<DescriptorPool>;

namespace exp {

// clang-format off
struct DescriptorBase {
    const vk::ShaderStageFlags shaderStages;
    const vk::DescriptorType descriptorType;

    DescriptorBase(vk::ShaderStageFlags shader_stages, vk::DescriptorType desc_type);
    DescriptorBase(const DescriptorBase&) = delete;
    DescriptorBase& operator=(const DescriptorBase&) = delete;
    DescriptorBase(DescriptorBase&&) noexcept = default;
    virtual ~DescriptorBase() = default;

    virtual void Initialize(DeviceH device, AllocatorH allocator) = 0;
    virtual void FillWriteWithResourcesInfo(
        vk::WriteDescriptorSet& write,
        std::vector< vk::DescriptorBufferInfo >& buffer_infos,
        std::vector< vk::DescriptorImageInfo >& image_infos
    ) const = 0;
};

struct DescriptorStatic : public DescriptorBase {
    DescriptorStatic(vk::ShaderStageFlags shader_stages, vk::DescriptorType desc_type);

    virtual void Initialize(DeviceH device, AllocatorH allocator) override;
    virtual auto GenBufferInfo() const -> vk::DescriptorBufferInfo;
    virtual auto GenImageInfo() const -> vk::DescriptorImageInfo;
    virtual void FillWriteWithResourcesInfo(
        vk::WriteDescriptorSet& write,
        std::vector< vk::DescriptorBufferInfo >& buffer_infos,
        std::vector< vk::DescriptorImageInfo >& image_infos
    ) const override;

    virtual void CreateBuffer(ren::DeviceH d, ren::AllocatorH a) = 0;
    virtual void CreateImage(ren::DeviceH d, ren::AllocatorH a) = 0;
    virtual void CreateView(ren::DeviceH d, ren::AllocatorH a) = 0;

    virtual bool HasBuffer() const noexcept;
    virtual bool HasImage() const noexcept;
    virtual bool HasView() const noexcept;

    [[nodiscard]] void* MapBuffer();
    void UnmapBuffer();
    
  protected:
    BufferU buffer;
    ImageU image;
    ImageViewU imageView;
};
// clang-format on

}; // namespace exp

// clang-format off
struct Descriptor {
    //! Main descriptor creation
    //! @param layout Shader layout number to connect to
    //! @param binding Shader binding number to connect to
    //! @param stages Shader stages to work in
    //! @param type Vulkan descriptor type enum value
    Descriptor(size_t layout, 
               size_t binding, 
               vk::DescriptorType type,
               vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex | 
                                             vk::ShaderStageFlagBits::eFragment,
               bool bindless = false)
        : layout{layout}
        , binding{binding}
        , shaderStages{stages}
        , type{type}
        , bindless{bindless}  
    {}

    virtual ~Descriptor();
    Descriptor(const Descriptor &) = delete;
    Descriptor &operator=(const Descriptor &) = delete;

    virtual void CreateBuffers(ren::DeviceH device, ren::AllocatorH allocator) = 0;
    virtual void CreateImages(ren::DeviceH device, ren::AllocatorH allocator) = 0;
    virtual void CreateViews(ren::DeviceH device, ren::AllocatorH allocator) = 0;

    virtual auto LayoutBinding() -> vk::DescriptorSetLayoutBinding;
    virtual auto BufferInfo(size_t idx=0) -> vk::DescriptorBufferInfo;
    virtual auto ImageInfo(size_t idx=0) -> vk::DescriptorImageInfo;

    auto IsBindless() -> bool;
    auto MapBuffer(size_t idx=0) -> void*;
    auto UnmapBuffer(size_t idx=0) -> void;
    auto Layout() const noexcept -> size_t; 
    auto Binding() const noexcept -> size_t; 
    auto ShaderStages() const noexcept -> vk::ShaderStageFlags; 
    auto DescriptorType() const noexcept -> vk::DescriptorType; 
    auto HasBuffer() const noexcept -> bool;
    auto HasImage() const noexcept -> bool;

  protected:
    const bool bindless;
    const size_t layout;
    const size_t binding;
    const vk::ShaderStageFlags shaderStages;
    const vk::DescriptorType type;

    std::vector<BufferU> buffers;
    std::vector<ImageU> images;
    std::vector<ImageViewU> imageViews;
};
// clang-format on

} // namespace nj::ren

#endif
