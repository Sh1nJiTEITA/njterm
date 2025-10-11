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

void DescriptorCells::CreateBuffers(
    ren::DeviceH device, ren::AllocatorH allocator
) {

    const size_t buffer_size_bytes = sizeof(buf::Cell) * textBuffer->Size();

    buffers.push_back(
        std::make_unique<Buffer>(
            device, allocator, buffer_size_bytes,
            vk::BufferUsageFlags(vk::BufferUsageFlagBits::eStorageBuffer),
            VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
            VmaAllocationCreateFlags{}
        )
    );
}

void DescriptorCells::CreateImages(
    ren::DeviceH device, ren::AllocatorH allocator
) {}
void DescriptorCells::CreateViews(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorCells::Update() {
    void* data = MapBuffer();
    textBuffer->MapDataTo(data);
    UnmapBuffer();
}

DescriptorCharactersMeta::DescriptorCharactersMeta(
    size_t layout, size_t binding, BufferU&& buf
)
    : Descriptor(
          layout, binding, vk::DescriptorType::eStorageBuffer,
          vk::ShaderStageFlagBits::eAll
      ) {
    buffers.push_back(std::move(buf));
}

void DescriptorCharactersMeta::CreateBuffers(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorCharactersMeta::CreateImages(
    ren::DeviceH device, ren::AllocatorH allocator
) {}
void DescriptorCharactersMeta::CreateViews(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

// void DescriptorCharactersMeta::Update(
//     const std::vector<SingleCharTextureData>& char_map
// ) {
//     void* data = MapBuffer();
//
//     memcpy(
//         data, char_map.data(), sizeof(SingleCharTextureData) *
//         char_map.size()
//     );
//
//     UnmapBuffer();
// }

BufferU CreateCharactersMetaBuffer(
    DeviceH d, AllocatorH a, const std::vector<SingleCharTextureData>& map
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
