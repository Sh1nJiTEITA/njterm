#include "nj_descriptor_texture.h"
#include "nj_allocator.h"
#include "nj_command_buffer.h"
#include "njlog.h"
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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
                                     CommandBufferH command_buffer,
                                     PhysicalDeviceH physical_device,
                                     size_t width, size_t height,
                                     std::vector<uint8_t> &&data)
    : Descriptor{layout, binding, vk::DescriptorType::eCombinedImageSampler,
                 stages},
      bitmap{std::move(data)}, commandBuffer{command_buffer},
      phDevice{physical_device}, textureWidth{width}, textureHeight{height}

{}

DescriptorTexture::~DescriptorTexture() {
    phDevice.reset();
    commandBuffer.reset();
}

void DescriptorTexture::CreateBuffer(ren::DeviceH device,
                                     ren::AllocatorH allocator) {
    using BitType = decltype(bitmap)::value_type;
    const size_t buf_sz = sizeof(BitType) * bitmap.size();
    buffer = std::make_unique<Buffer>(
        device, allocator, buf_sz, vk::BufferUsageFlagBits::eTransferSrc,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
    void *data = buffer->Map();
    memcpy(data, bitmap.data(), buf_sz);
    buffer->Unmap();
}

void DescriptorTexture::CreateImage(ren::DeviceH device,
                                    ren::AllocatorH allocator) {
    image = std::make_unique<Image>(
        device, allocator, textureWidth, textureHeight, 1, vk::Format::eR8Srgb,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);
}

void DescriptorTexture::CreateView(ren::DeviceH device,
                                   ren::AllocatorH allocator) {}

void DescriptorTexture::BeginCommandBufferSingleCommand() {
    auto info = vk::CommandBufferBeginInfo{}.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer->Handle()->begin(info);
}

void DescriptorTexture::TransitionImageLayout(vk::ImageLayout o,
                                              vk::ImageLayout n) {
    BeginCommandBufferSingleCommand();
    // clang-format off
    auto bar = vk::ImageMemoryBarrier{}
                   .setOldLayout(o)
                   .setNewLayout(n)
                   .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                   .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                   .setImage(image->CHandle())
                   .setSubresourceRange(
                       vk::ImageSubresourceRange{}
                           .setAspectMask(vk::ImageAspectFlagBits::eColor)
                           .setBaseMipLevel(0)
                           .setLevelCount(1)
                           .setBaseArrayLayer(0)
                           .setLayerCount(1)
                   )
                   .setDstAccessMask(vk::AccessFlagBits::eNone)
                   .setSrcAccessMask(vk::AccessFlagBits::eNone)
                   ;

    // clang-format on
    vk::PipelineStageFlags src_stage, dst_stage;
    if (o == vk::ImageLayout::eUndefined &&
        n == vk::ImageLayout::eTransferDstOptimal) {
        bar.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        dst_stage = vk::PipelineStageFlagBits::eTransfer;
        image->Layout() = vk::ImageLayout::eTransferDstOptimal;
    } else if (o == vk::ImageLayout::eTransferDstOptimal &&
               n == vk::ImageLayout::eShaderReadOnlyOptimal) {
        bar.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        bar.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        src_stage = vk::PipelineStageFlagBits::eTransfer;
        dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        image->Layout() = vk::ImageLayout::eShaderReadOnlyOptimal;
    } else {
        log::Error("Wrong layouts to translate texture image");
    }
    commandBuffer->Handle()->pipelineBarrier(src_stage, dst_stage, {}, {}, {},
                                             std::array{bar});
    EndCommandBufferSingleCommand();
}

void DescriptorTexture::EndCommandBufferSingleCommand() {
    commandBuffer->Handle()->end();
    auto command_buffers = std::array{commandBuffer->CHandle()};
    auto info = vk::SubmitInfo{}.setCommandBuffers(command_buffers);
    auto &queue = phDevice->GraphicsQueue();
    queue.submit(info, {});
    queue.waitIdle();
}

} // namespace nj::ren
