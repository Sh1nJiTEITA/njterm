#include "nj_descriptor_grid.h"
#include "nj_log_scope.h"
#include "njlog.h"
#include <cstring>
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

namespace exp {

DescriptorGrid::DescriptorGrid(vk::ShaderStageFlags shader_stages)
    : DescriptorStatic{shader_stages, vk::DescriptorType::eUniformBuffer} {}

void DescriptorGrid::CreateBuffer(
    ren::DeviceH device, ren::AllocatorH allocator
) {
    DEBUG_SCOPE_A("Creating buffer for DescriptorGrid");
    buffer = std::make_unique<Buffer>(
        device, allocator, sizeof(Data),
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eUniformBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, VmaAllocationCreateFlags{}
    );
}
void DescriptorGrid::CreateImage(
    ren::DeviceH device, ren::AllocatorH allocator
) {
    DEBUG_SCOPE_A("Skipping image creation for DescriptorGrid");
}

void DescriptorGrid::CreateView(
    ren::DeviceH device, ren::AllocatorH allocator
) {
    DEBUG_SCOPE_A("Skipping image views creation for DescriptorGrid");
}

void DescriptorGrid::Update(
    const glm::ivec2& ext, const glm::ivec2 face_size,
    const glm::ivec2& page_size
) {
    void* data = MapBuffer();
    Data tmp{.extent = ext, .faceSize = face_size, .pageSize = page_size};
    memcpy(data, &tmp, sizeof(Data));
    UnmapBuffer();
}

} // namespace exp

DescriptorGrid::DescriptorGrid(size_t layout, size_t binding)
    : Descriptor(layout, binding, vk::DescriptorType::eUniformBuffer) {}

// clang-format off
void DescriptorGrid::CreateBuffers(ren::DeviceH device,
                                  ren::AllocatorH allocator) {
    buffers.push_back(std::make_unique<Buffer>(
        device, 
        allocator, 
        sizeof(Data),
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eUniformBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
        VmaAllocationCreateFlags{}
    ));

    void* data = buffers.back()->Map();
    // Data tmp { .color = { 1.f, 0.f, 0.f } };
    // memcpy(data, &tmp, sizeof(tmp));
    buffers.back()->Unmap();
}
// clang-format on

void DescriptorGrid::CreateImages(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorGrid::CreateViews(
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
