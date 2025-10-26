#pragma once
#ifndef NJ_DESCRIPTOR_TEXTURE_H
#define NJ_DESCRIPTOR_TEXTURE_H
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_physical_device.h"
#include "nj_sampler.h"

namespace nj::ren {

// clang-format off


struct DescriptorTexture : public DescriptorStatic {
    const uint32_t width;
    const uint32_t height;
    const uint32_t stride;

    //! Creation with internal buffer handling 
    //! Image/texture data from `data` will be
    //! copied to internal VkBuffer once
    //! @param stages Stages to have access to
    //! @param command_buffer Command buffer to upload data
    //! @param physical_device Physical device to process upload
    //! @param sampler Sampler to handle texture properties
    //! @param width Texture width (number of pixels)
    //! @param height Texture height (number of pixels)
    //! @param stride Stride for row inside data (for alligned data, e.g. FreeType)
    //! @param data Data to use inside texture
    DescriptorTexture(vk::ShaderStageFlags stages, 
                      CommandBufferH command_buffer,
                      PhysicalDeviceH physical_device,
                      SamplerH sampler,
                      uint32_t width,
                      uint32_t height,
                      uint32_t stride,
                      const std::vector<uint8_t>& data);

    //! Creation with external buffer
    //! @param stages Stages to have access to
    //! @param command_buffer Command buffer to upload data
    //! @param physical_device Physical device to process upload
    //! @param sampler Sampler to handle texture properties
    //! @param width Texture width (number of pixels)
    //! @param height Texture height (number of pixels)
    //! @param stride Stride for row inside data (for alligned data, e.g. FreeType)
    //! @param buffer Allocated and mapped VkBuffer
    DescriptorTexture(vk::ShaderStageFlags stages, 
                      CommandBufferH command_buffer,
                      PhysicalDeviceH physical_device,
                      SamplerH sampler,
                      uint32_t width,
                      uint32_t height,
                      std::unique_ptr<Buffer>&& buffer);

    virtual ~DescriptorTexture();

    virtual void CreateBuffer(ren::DeviceH d, ren::AllocatorH a) override;
    virtual void CreateImage(ren::DeviceH d, ren::AllocatorH a) override;
    virtual void CreateView(ren::DeviceH d, ren::AllocatorH a) override;

    virtual auto GenImageInfo() const -> vk::DescriptorImageInfo override;

private:
    // void BeginCommandBufferSingleCommand();
    // void TransitionImageLayout(vk::ImageLayout o, vk::ImageLayout n);
    // void CopyBufferToImage();
    // void EndCommandBufferSingleCommand();

private: 
    CommandBufferH commandBuffer;
    PhysicalDeviceH phDevice;
    SamplerH sampler;

    std::vector<uint8_t> bitmap;
};

// clang-format on

} // namespace nj::ren

#endif
