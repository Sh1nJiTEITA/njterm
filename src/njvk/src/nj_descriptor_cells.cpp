#include "nj_descriptor_cells.h"
#include "nj_descriptor.h"
#include "nj_text_buffer.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

DescriptorCells::DescriptorCells(
    vk::ShaderStageFlags stages,
    buf::TextBufferH buf
)
    : DescriptorStatic(
          vk::ShaderStageFlagBits::eAll,
          vk::DescriptorType::eStorageBuffer
      )
    , textBuffer(buf) {}

void DescriptorCells::CreateBuffer(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {

    const size_t buffer_size_bytes = sizeof(buf::Cell) * textBuffer->Size();

    buffer = std::make_unique<Buffer>(
        device, allocator, buffer_size_bytes,
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eStorageBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, VmaAllocationCreateFlags{}
    );
}

void DescriptorCells::CreateImage(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {}
void DescriptorCells::CreateView(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {}

void DescriptorCells::Update() {
    void* data = MapBuffer();
    textBuffer->MapDataTo(data);
    UnmapBuffer();
}

DescriptorCharactersMeta::DescriptorCharactersMeta(
    vk::ShaderStageFlags stages,
    BufferU&& buf
)
    : DescriptorStatic(
          vk::ShaderStageFlagBits::eAll,
          vk::DescriptorType::eStorageBuffer
      ) {
    buffer = std::move(buf);
}

void DescriptorCharactersMeta::CreateBuffer(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {}
void DescriptorCharactersMeta::CreateImage(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {}
void DescriptorCharactersMeta::CreateView(
    ren::DeviceH device,
    ren::AllocatorH allocator
) {}

BufferU CreateCharactersMetaBuffer(
    DeviceH d,
    AllocatorH a,
    const std::vector<SingleCharTextureData>& map
) {
    const size_t buffer_size_bytes = sizeof(SingleCharTextureData) * map.size();

    auto buf = std::make_unique<Buffer>(
        d, a, buffer_size_bytes,
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eStorageBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, VmaAllocationCreateFlags{}
    );

    void* data = buf->Map();
    memcpy(data, map.data(), buffer_size_bytes);
    buf->Unmap();
    return buf;
}

BufferU CreateCharactersMetaBuffer(DeviceH d, AllocatorH a, size_t cells) {
    const size_t buffer_size_bytes = sizeof(SingleCharTextureData) * cells;

    return std::make_unique<Buffer>(
        d, a, buffer_size_bytes,
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eStorageBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU, VmaAllocationCreateFlags{}
    );
}

} // namespace nj::ren
