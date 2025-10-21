#include "nj_descriptor_texture_array.h"
#include "nj_descriptor.h"

namespace nj::ren {
DescriptorTextureArray::DescriptorTextureArray(
    size_t layout, size_t binding, vk::ShaderStageFlags stages,
    std::vector<BufferU>&& texture_buffers
)
    : Descriptor(
          layout, binding, vk::DescriptorType::eCombinedImageSampler, stages
      ) {
    buffers = std::move(texture_buffers);
}

void DescriptorTextureArray::CreateBuffers(
    DeviceH device, AllocatorH allocator
) {}

void DescriptorTextureArray::CreateImages(
    DeviceH device, AllocatorH allocator
) {}

void DescriptorTextureArray::CreateViews(DeviceH device, AllocatorH allocator) {
}

}; // namespace nj::ren
