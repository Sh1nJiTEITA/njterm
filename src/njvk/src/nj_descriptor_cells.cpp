#include "nj_descriptor_cells.h"
#include "nj_descriptor.h"
#include "nj_text_buffer.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

DescriptorCells::DescriptorCells(
    size_t layout, size_t binding, buf::TextBufferH buf
)
    : Descriptor(
          layout, binding, vk::DescriptorType::eStorageBuffer,
          vk::ShaderStageFlagBits::eAll
      ),
      textBuffer(buf) {}

void DescriptorCells::CreateBuffer(
    ren::DeviceH device, ren::AllocatorH allocator
) {

    const size_t buffer_size_bytes = sizeof(buf::Cell) * textBuffer->Size();

    buffer = std::make_unique<Buffer>(
        device, allocator, buffer_size_bytes,
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eStorageBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, VmaAllocationCreateFlags{}
    );
}

void DescriptorCells::CreateImage(
    ren::DeviceH device, ren::AllocatorH allocator
) {}
void DescriptorCells::CreateView(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorCells::Update() {
    void* data = MapBuffer();
    textBuffer->MapDataTo(data);
    UnmapBuffer();
}

} // namespace nj::ren
