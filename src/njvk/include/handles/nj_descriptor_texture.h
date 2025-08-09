#pragma once
#ifndef NJ_DESCRIPTOR_TEXTURE_H
#define NJ_DESCRIPTOR_TEXTURE_H
#include "nj_descriptor.h"

namespace nj::ren {

// clang-format off

struct DescriptorTexture : public Descriptor {
    DescriptorTexture(size_t layout, size_t binding, vk::ShaderStageFlags stages, std::vector<uint8_t>&& data);

    virtual void CreateBuffer(DeviceH device, AllocatorH allocator) override;
    virtual void CreateImage(DeviceH device, AllocatorH allocator) override;
    virtual void CreateView(DeviceH device, AllocatorH allocator) override;

private: 
    std::vector<uint8_t> bitmap;
};
// clang-format on

} // namespace nj::ren

#endif
