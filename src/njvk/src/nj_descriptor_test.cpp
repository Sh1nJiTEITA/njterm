#include "nj_descriptor_test.h"

namespace nj::ren {

DescriptorTest::DescriptorTest(size_t layout, size_t binding)
    : Descriptor(layout, binding, vk::DescriptorType::eUniformBuffer) {}

// clang-format off
void DescriptorTest::CreateBuffers(ren::DeviceH device,
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
    Data tmp { .color = { 1.f, 0.f, 0.f } };
    memcpy(data, &tmp, sizeof(tmp));
    buffers.back()->Unmap();
}
// clang-format on

void DescriptorTest::CreateImages(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

void DescriptorTest::CreateViews(
    ren::DeviceH device, ren::AllocatorH allocator
) {}

} // namespace nj::ren
