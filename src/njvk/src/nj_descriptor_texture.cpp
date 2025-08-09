#include "nj_descriptor_texture.h"

namespace nj::ren {
//
// DescriptorTest::DescriptorTest()
//     : Descriptor(0, 0, vk::DescriptorType::eUniformBuffer) {}
//
// // clang-format off
// void DescriptorTest::CreateBuffer(ren::DeviceH device,
//                                   ren::AllocatorH allocator) {
//     buffer = std::make_unique<Buffer>(
//         device,
//         allocator,
//         sizeof(Data),
//         vk::BufferUsageFlags(vk::BufferUsageFlagBits::eUniformBuffer),
//         VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
//         VmaAllocationCreateFlags{}
//     );
//
//     void* data = buffer->Map();
//     Data tmp { .color = { 1.f, 0.f, 0.f } };
//     memcpy(data, &tmp, sizeof(tmp));
//     buffer->Unmap();
// }
// // clang-format on
//
// void DescriptorTest::CreateImage(ren::DeviceH device,
//                                  ren::AllocatorH allocator) {}
//
// void DescriptorTest::CreateView(ren::DeviceH device,
//                                 ren::AllocatorH allocator) {}
//
//

DescriptorTexture::DescriptorTexture(size_t layout, size_t binding,
                                     vk::ShaderStageFlags stages,
                                     std::vector<uint8_t> &&data)
    : Descriptor{layout, binding, vk::DescriptorType::eCombinedImageSampler,
                 stages},
      bitmap{std::move(data)} {}

void DescriptorTexture::CreateBuffer(ren::DeviceH device,
                                     ren::AllocatorH allocator) {
    const size_t buf_sz = sizeof(decltype(bitmap)::value_type) * bitmap.size();

    // buffer = std::make_unique<Buffer>(device, allocator,
    //
    // );
}

void DescriptorTexture::CreateImage(ren::DeviceH device,
                                    ren::AllocatorH allocator) {}

void DescriptorTexture::CreateView(ren::DeviceH device,
                                   ren::AllocatorH allocator) {}

} // namespace nj::ren
