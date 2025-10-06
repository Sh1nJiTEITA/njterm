#pragma once
#ifndef NJ_DESCRIPTOR_TEXTURE_H
#define NJ_DESCRIPTOR_TEXTURE_H
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_physical_device.h"
#include "nj_sampler.h"

namespace nj::ren {

// clang-format off

struct DescriptorTexture : public Descriptor {
    //! Creation with internal buffer handling 
    //! Image/texture data from `data` will be
    //! copied to internal VkBuffer once
    //! @param layout Layout for descriptor set
    //! @param binding Binding to access inside shader
    //! @param stages Stages to have access to
    //! @param command_buffer Command buffer to upload data
    //! @param physical_device Physical device to process upload
    //! @param sampler Sampler to handle texture properties
    //! @param width Texture width (number of pixels)
    //! @param height Texture height (number of pixels)
    //! @param stride Stride for row inside data (for alligned data, e.g. FreeType)
    //! @param data Data to use inside texture
    DescriptorTexture(size_t layout, size_t binding, 
                      vk::ShaderStageFlags stages, 
                      CommandBufferH command_buffer,
                      PhysicalDeviceH physical_device,
                      SamplerH sampler,
                      size_t width,
                      size_t height,
                      size_t stride,
                      const std::vector<uint8_t>& data);

    //! Creation with external buffer
    //! @param layout Layout for descriptor set
    //! @param binding Binding to access inside shader
    //! @param stages Stages to have access to
    //! @param command_buffer Command buffer to upload data
    //! @param physical_device Physical device to process upload
    //! @param sampler Sampler to handle texture properties
    //! @param width Texture width (number of pixels)
    //! @param height Texture height (number of pixels)
    //! @param stride Stride for row inside data (for alligned data, e.g. FreeType)
    //! @param buffer Allocated and mapped VkBuffer
    DescriptorTexture(size_t layout, size_t binding, 
                      vk::ShaderStageFlags stages, 
                      CommandBufferH command_buffer,
                      PhysicalDeviceH physical_device,
                      SamplerH sampler,
                      size_t width,
                      size_t height,
                      std::unique_ptr<Buffer>&& buffer);

    virtual ~DescriptorTexture();

    virtual void CreateBuffer(DeviceH device, AllocatorH allocator) override;
    virtual void CreateImage(DeviceH device, AllocatorH allocator) override;
    virtual void CreateView(DeviceH device, AllocatorH allocator) override;
    virtual auto ImageInfo() -> vk::DescriptorImageInfo override;

private:
    void BeginCommandBufferSingleCommand();
    void TransitionImageLayout(vk::ImageLayout o, vk::ImageLayout n);
    void CopyBufferToImage();
    void EndCommandBufferSingleCommand();

private: 
    CommandBufferH commandBuffer;
    PhysicalDeviceH phDevice;
    SamplerH sampler;

    size_t width;
    size_t height;
    size_t stride;

    std::unique_ptr<Buffer> textureBuffer;
    std::vector<uint8_t> bitmap;
};
// clang-format on

} // namespace nj::ren

#endif
