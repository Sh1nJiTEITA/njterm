#pragma once
#ifndef NJ_DESCRIPTOR_H
#define NJ_DESCRIPTOR_H

#include "nj_allocator.h"
#include "nj_buffer.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "nj_image.h"
#include "nj_image_view.h"
#include "nj_sampler.h"
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

struct DescriptorBase {
    const vk::ShaderStageFlags shaderStages;
    const vk::DescriptorType descriptorType;

    DescriptorBase(vk::ShaderStageFlags stages, vk::DescriptorType desc_type);
    DescriptorBase(const DescriptorBase&) = delete;
    DescriptorBase& operator=(const DescriptorBase&) = delete;
    DescriptorBase(DescriptorBase&&) noexcept = default;
    virtual ~DescriptorBase() = default;

    virtual void Initialize(DeviceH device, AllocatorH allocator) = 0;
    virtual uint32_t Count() const = 0;
    virtual void FillWriteWithResourcesInfo(
        vk::WriteDescriptorSet& write,
        std::vector<vk::DescriptorBufferInfo>& buffer_infos,
        std::vector<vk::DescriptorImageInfo>& image_infos
    ) const = 0;
};

struct DescriptorStatic : public DescriptorBase {
    DescriptorStatic(
        vk::ShaderStageFlags shader_stages,
        vk::DescriptorType desc_type
    );

    void SomeTestFunc();
    virtual void Initialize(DeviceH device, AllocatorH allocator) override;
    virtual auto GenBufferInfo() const -> vk::DescriptorBufferInfo;
    virtual auto GenImageInfo() const -> vk::DescriptorImageInfo;

    virtual void FillWriteWithResourcesInfo(
        vk::WriteDescriptorSet& write,
        std::vector<vk::DescriptorBufferInfo>& buffer_infos,
        std::vector<vk::DescriptorImageInfo>& image_infos
    ) const override;
    virtual uint32_t Count() const override;

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

} // namespace nj::ren

#endif
