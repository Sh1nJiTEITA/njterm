#ifndef NJ_DESCRIPTOR_TEXTURE_ARRAY_H
#define NJ_DESCRIPTOR_TEXTURE_ARRAY_H

#include "nj_descriptor.h"

namespace nj::ren {

struct DescriptorTextureArray : public Descriptor {

    DescriptorTextureArray(
        size_t layout, size_t binding, vk::ShaderStageFlags stages,
        std::vector<BufferU>&& texture_buffers
    );

    virtual void CreateBuffers(DeviceH device, AllocatorH allocator) override;
    virtual void CreateImages(DeviceH device, AllocatorH allocator) override;
    virtual void CreateViews(DeviceH device, AllocatorH allocator) override;
};

}; // namespace nj::ren

#endif
