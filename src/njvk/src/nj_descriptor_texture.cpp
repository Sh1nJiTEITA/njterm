#include "nj_descriptor_texture.h"
#include "nj_allocator.h"
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "njlog.h"
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

namespace exp {

DescriptorTexture::DescriptorTexture(
    vk::ShaderStageFlags stages, CommandBufferH command_buffer,
    PhysicalDeviceH physical_device, SamplerH sampler, uint32_t width,
    uint32_t height, uint32_t stride, const std::vector<uint8_t>& data
)
    : DescriptorStatic{stages, vk::DescriptorType::eCombinedImageSampler},
      bitmap{data},
      commandBuffer{command_buffer},
      phDevice{physical_device},
      sampler{sampler},
      width{width},
      height{height},
      stride{stride} {
    assert(data.size() == width * height);
}

DescriptorTexture::DescriptorTexture(
    vk::ShaderStageFlags stages, CommandBufferH command_buffer,
    PhysicalDeviceH physical_device, SamplerH sampler, uint32_t width,
    uint32_t height, std::unique_ptr<Buffer>&& buffer
)
    : DescriptorStatic{stages, vk::DescriptorType::eCombinedImageSampler},
      bitmap{},
      commandBuffer{command_buffer},
      phDevice{physical_device},
      sampler{sampler},
      width{width},
      height{height},
      stride{0} {
    this->buffer = std::move(buffer);
}

DescriptorTexture::~DescriptorTexture() {
    log::Debug("Destroying DescriptorTexture...");
    buffer.reset();
    phDevice.reset();
    commandBuffer.reset();
    sampler.reset();
}

void DescriptorTexture::CreateBuffer(
    ren::DeviceH device, ren::AllocatorH allocator
) {
    if (!buffer && stride) {
        using BitType = decltype(bitmap)::value_type;
        const size_t buf_sz = sizeof(BitType) * bitmap.size();
        buffer = std::make_unique<Buffer>(
            device, allocator, buf_sz, vk::BufferUsageFlagBits::eTransferSrc,
            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
                | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
        );
        void* data = buffer->Map();
        uint8_t* dst = static_cast<uint8_t*>(data);

        for (int y = 0; y < height; ++y) {
            memcpy(dst + y * width, bitmap.data() + y * stride, width);
        }
        buffer->Unmap();
    }
}

// clang-format off
void DescriptorTexture::CreateImage(ren::DeviceH device,
                                    ren::AllocatorH allocator) {
    image = std::make_unique<Image>(
        device, allocator, width, height, 1, vk::Format::eR8Unorm,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);

    TransitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    CopyBufferToImage();
    TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}
// clang-format off

void DescriptorTexture::CreateView(ren::DeviceH device,
                                   ren::AllocatorH allocator) {
    vk::ImageViewCreateInfo info{};
    info.image = image->CHandle();
    info.viewType = vk::ImageViewType::e2D;
    info.format = vk::Format::eR8Unorm;
    info.components.setR(vk::ComponentSwizzle::eR);
    info.components.setG(vk::ComponentSwizzle::eR);
    info.components.setB(vk::ComponentSwizzle::eR);
    info.components.setA(vk::ComponentSwizzle::eR);
    info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    imageView = std::make_unique<ImageView>(device->Handle().createImageViewUnique(info));
}


auto DescriptorTexture::GenImageInfo() const -> vk::DescriptorImageInfo { 
    auto info = DescriptorStatic::GenImageInfo(); 
    info.setSampler(sampler->Handle());
    return info;
}


void DescriptorTexture::BeginCommandBufferSingleCommand() {
    auto info = vk::CommandBufferBeginInfo{}.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    commandBuffer->Handle().begin(info);
}

void DescriptorTexture::TransitionImageLayout(
    vk::ImageLayout o, vk::ImageLayout n
) {
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
    if (o == vk::ImageLayout::eUndefined
        && n == vk::ImageLayout::eTransferDstOptimal) {
        bar.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        dst_stage = vk::PipelineStageFlagBits::eTransfer;
        image->Layout() = vk::ImageLayout::eTransferDstOptimal;
    } else if (o == vk::ImageLayout::eTransferDstOptimal
               && n == vk::ImageLayout::eShaderReadOnlyOptimal) {
        bar.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        bar.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        src_stage = vk::PipelineStageFlagBits::eTransfer;
        dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        image->Layout() = vk::ImageLayout::eShaderReadOnlyOptimal;
    } else {
        log::Error("Wrong layouts to translate texture image");
    }
    commandBuffer->Handle().pipelineBarrier(
        src_stage, dst_stage, {}, {}, {}, std::array{bar}
    );
    EndCommandBufferSingleCommand();
}

// clang-format off
void DescriptorTexture::CopyBufferToImage() {
    BeginCommandBufferSingleCommand();
    auto info = vk::BufferImageCopy{}
        .setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource(vk::ImageSubresourceLayers{}
                                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                .setMipLevel(0)
                                .setBaseArrayLayer(0)
                                .setLayerCount(1))
        .setImageOffset({ 0, 0, 0 })
        .setImageExtent({ 
            static_cast< uint32_t > ( width ),
            static_cast< uint32_t > ( height ) ,
            1
        })
        ;
    commandBuffer->Handle().copyBufferToImage(
        buffer->CHandle(), image->CHandle(), 
        vk::ImageLayout::eTransferDstOptimal, info
    );
    EndCommandBufferSingleCommand();
    // buffer.reset();
}
// clang-format off

void DescriptorTexture::EndCommandBufferSingleCommand() {
    commandBuffer->Handle().end();
    auto command_buffers = std::array{commandBuffer->Handle()};
    auto info = vk::SubmitInfo{}.setCommandBuffers(command_buffers);
    auto &queue = phDevice->GraphicsQueue();
    queue.submit(info, {});
    queue.waitIdle();
}





}; // namespace exp

DescriptorTexture::DescriptorTexture(
    size_t layout, size_t binding, vk::ShaderStageFlags stages,
    CommandBufferH command_buffer, PhysicalDeviceH physical_device,
    SamplerH sampler, size_t width, size_t height, size_t stride,
    const std::vector<uint8_t>& data
)
    : Descriptor{layout, binding, vk::DescriptorType::eCombinedImageSampler, stages},
      bitmap{data},
      commandBuffer{command_buffer},
      phDevice{physical_device},
      sampler{sampler},
      width{width},
      height{height},
      stride{stride} {

    assert(data.size() == width * height);
}

DescriptorTexture::DescriptorTexture(
    size_t layout, size_t binding, vk::ShaderStageFlags stages,
    CommandBufferH command_buffer, PhysicalDeviceH physical_device,
    SamplerH sampler, size_t width, size_t height,
    std::unique_ptr<Buffer>&& buffer
)
    : Descriptor{layout, binding, vk::DescriptorType::eCombinedImageSampler, stages},
      bitmap{},
      commandBuffer{command_buffer},
      phDevice{physical_device},
      sampler{sampler},
      width{width},
      height{height},
      stride{0},
      textureBuffer{std::move(buffer)} {}

DescriptorTexture::~DescriptorTexture() {
    log::Debug("Destroying DescriptorTexture...");
    textureBuffer.reset();
    phDevice.reset();
    commandBuffer.reset();
    sampler.reset();
}

void DescriptorTexture::CreateBuffers(
    ren::DeviceH device, ren::AllocatorH allocator
) {
    if (!textureBuffer && stride) {
        using BitType = decltype(bitmap)::value_type;
        const size_t buf_sz = sizeof(BitType) * bitmap.size();
        textureBuffer = std::make_unique<Buffer>(
            device, allocator, buf_sz, vk::BufferUsageFlagBits::eTransferSrc,
            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
                | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
        );
        void* data = textureBuffer->Map();
        uint8_t* dst = static_cast<uint8_t*>(data);

        for (int y = 0; y < height; ++y) {
            memcpy(dst + y * width, bitmap.data() + y * stride, width);
        }
        textureBuffer->Unmap();
    }
    // void *data = textureBuffer->Map();
    // memcpy(data, bitmap.data(), buf_sz);
    // textureBuffer->Unmap();
}

// clang-format off
void DescriptorTexture::CreateImages(ren::DeviceH device,
                                    ren::AllocatorH allocator) {
    images.push_back(std::make_unique<Image>(
        device, allocator, width, height, 1, vk::Format::eR8Unorm,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO));

    TransitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    CopyBufferToImage();
    TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}
// clang-format off

void DescriptorTexture::CreateViews(ren::DeviceH device,
                                   ren::AllocatorH allocator) {
    vk::ImageViewCreateInfo info{};
    info.image = images.back()->CHandle();
    info.viewType = vk::ImageViewType::e2D;
    info.format = vk::Format::eR8Unorm;
    info.components.setR(vk::ComponentSwizzle::eR);
    info.components.setG(vk::ComponentSwizzle::eR);
    info.components.setB(vk::ComponentSwizzle::eR);
    info.components.setA(vk::ComponentSwizzle::eR);
    info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    imageViews.push_back(std::make_unique<ImageView>(device->Handle().createImageViewUnique(info)));
}

auto DescriptorTexture::ImageInfo(size_t idx) -> vk::DescriptorImageInfo { 
    auto image_info = Descriptor::ImageInfo(idx);
    image_info.setSampler(sampler->CHandle());
    return image_info;
}

void DescriptorTexture::BeginCommandBufferSingleCommand() {
    auto info = vk::CommandBufferBeginInfo{}.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer->Handle().begin(info);
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
                   .setImage(images.back()->CHandle())
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
    if (o == vk::ImageLayout::eUndefined
        && n == vk::ImageLayout::eTransferDstOptimal) {
        bar.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        dst_stage = vk::PipelineStageFlagBits::eTransfer;
        images.back()->Layout() = vk::ImageLayout::eTransferDstOptimal;
    } else if (o == vk::ImageLayout::eTransferDstOptimal
               && n == vk::ImageLayout::eShaderReadOnlyOptimal) {
        bar.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        bar.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        src_stage = vk::PipelineStageFlagBits::eTransfer;
        dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        images.back()->Layout() = vk::ImageLayout::eShaderReadOnlyOptimal;
    } else {
        log::Error("Wrong layouts to translate texture image");
    }
    commandBuffer->Handle().pipelineBarrier(
        src_stage, dst_stage, {}, {}, {}, std::array{bar}
    );
    EndCommandBufferSingleCommand();
}

// clang-format off
void DescriptorTexture::CopyBufferToImage() {
    BeginCommandBufferSingleCommand();
    auto info = vk::BufferImageCopy{}
        .setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource(vk::ImageSubresourceLayers{}
                                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                .setMipLevel(0)
                                .setBaseArrayLayer(0)
                                .setLayerCount(1))
        .setImageOffset({ 0, 0, 0 })
        .setImageExtent({ 
            static_cast< uint32_t > ( width ),
            static_cast< uint32_t > ( height ) ,
            1
        })
        ;
    commandBuffer->Handle().copyBufferToImage(
        textureBuffer->CHandle(), images.back()->CHandle(), 
        vk::ImageLayout::eTransferDstOptimal, info
    );
    EndCommandBufferSingleCommand();
}
// clang-format off

void DescriptorTexture::EndCommandBufferSingleCommand() {
    commandBuffer->Handle().end();
    auto command_buffers = std::array{commandBuffer->Handle()};
    auto info = vk::SubmitInfo{}.setCommandBuffers(command_buffers);
    auto &queue = phDevice->GraphicsQueue();
    queue.submit(info, {});
    queue.waitIdle();
}

} // namespace nj::ren
