#pragma once
#include "nj_physical_device.h"
#include <vulkan/vulkan_enums.hpp>
#ifndef NJ_DESCRIPTOR_TEXTURE_H
#define NJ_DESCRIPTOR_TEXTURE_H
#include "nj_command_buffer.h"
#include "nj_descriptor.h"

namespace nj::ren {

// clang-format off

struct DescriptorTexture : public Descriptor {
    DescriptorTexture(size_t layout, size_t binding, 
                      vk::ShaderStageFlags stages, 
                      CommandBufferH command_buffer,
                      PhysicalDeviceH physical_device,
                      size_t width,
                      size_t height,
                      std::vector<uint8_t>&& data);
    ~DescriptorTexture();
    virtual void CreateBuffer(DeviceH device, AllocatorH allocator) override;
    virtual void CreateImage(DeviceH device, AllocatorH allocator) override;
    virtual void CreateView(DeviceH device, AllocatorH allocator) override;

private:
    void BeginCommandBufferSingleCommand();
    void TransitionImageLayout(vk::ImageLayout o, vk::ImageLayout n);
    void EndCommandBufferSingleCommand();

private: 
    CommandBufferH commandBuffer;
    PhysicalDeviceH phDevice;

    size_t textureWidth;
    size_t textureHeight;
    std::vector<uint8_t> bitmap;
};
// clang-format on

} // namespace nj::ren

#endif
