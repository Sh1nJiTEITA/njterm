#include "nj_descriptor_test.h"

namespace nj::ren {

DescriptorTest::DescriptorTest()
    : Descriptor(0, 0, vk::DescriptorType::eUniformBuffer) {}

// clang-format off
void DescriptorTest::CreateBuffer(ren::DeviceH device,
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
    Data tmp { .color = { 1.f, 0.f, 0.f } };
    memcpy(data, &tmp, sizeof(tmp));
    buffer->Unmap();
}
// clang-format on

void DescriptorTest::CreateImage(ren::DeviceH device,
                                 ren::AllocatorH allocator) {}

void DescriptorTest::CreateView(ren::DeviceH device,
                                ren::AllocatorH allocator) {}

} // namespace nj::ren
