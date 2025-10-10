#include "nj_descriptor_grid.h"
#include <cstring>

namespace nj::ren {

DescriptorGrid::DescriptorGrid(size_t layout, size_t binding)
    : Descriptor(layout, binding, vk::DescriptorType::eUniformBuffer) {}

// clang-format off
void DescriptorGrid::CreateBuffer(ren::DeviceH device,
                                  ren::AllocatorH allocator) {
    buffer = std::make_unique<Buffer>(
        device, 
        allocator, 
        sizeof(Data),
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eUniformBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
        VmaAllocationCreateFlags{}
    );

    void* data = buffer->Map();
    // Data tmp { .color = { 1.f, 0.f, 0.f } };
    // memcpy(data, &tmp, sizeof(tmp));
    buffer->Unmap();
}
// clang-format on

void DescriptorGrid::CreateImage(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorGrid::CreateView(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorGrid::Update(
    const glm::ivec2& ext, const glm::ivec2 face_size,
    const glm::ivec2& page_size
) {
    void* data = MapBuffer();
    Data tmp{.extent = ext, .faceSize = face_size, .pageSize = page_size};
    memcpy(data, &tmp, sizeof(Data));
    UnmapBuffer();
}

} // namespace nj::ren
