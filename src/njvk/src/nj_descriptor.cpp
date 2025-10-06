#include "nj_descriptor.h"
#include "njlog.h"
#include "njvkutils.h"
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

// clang-format off
DescriptorPool::DescriptorPool(ren::DeviceH device) {
    ren::VarHandles h{};

    auto info = vk::DescriptorPoolCreateInfo{}
        .setPoolSizes(
            h.Handle(std::vector{ 
                vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 10},
                vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 10},
                vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 10},
            })
        )
        .setMaxSets(10)
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        ;

    handle = device->Handle().createDescriptorPoolUnique(info);
}
// clang-format off


auto DescriptorPool::HandleName() const noexcept -> std::string {
    return "DescriptorPool";
}

Descriptor::~Descriptor() { }

auto Descriptor::LayoutBinding() -> vk::DescriptorSetLayoutBinding {
    return vk::DescriptorSetLayoutBinding{}
        .setBinding(binding)
        .setDescriptorType(type)
        .setDescriptorCount(1)
        .setStageFlags(shaderStages)
        ;
}

auto Descriptor::BufferInfo() -> vk::DescriptorBufferInfo {
    if (buffer) { 
        return  vk::DescriptorBufferInfo {}
            .setBuffer(buffer->CHandle())
            .setOffset(0)
            .setRange(buffer->InitialSize())
            ;
    }
    return {};
}

auto Descriptor::ImageInfo() -> vk::DescriptorImageInfo {
    if (image) { 
        return vk::DescriptorImageInfo{}
            .setImageView(imageView->Handle())
            .setImageLayout(image->Layout())
            ;
    }
    return {};
}

auto Descriptor::MapBuffer() -> void* { 
    if (!buffer) { 
        log::FatalExit("Cant map descriptor buffer. Buffer has invalid handle");
    }
    return buffer->Map();
}

auto Descriptor::UnmapBuffer() -> void {
    buffer->Unmap();
}


auto Descriptor::Layout() const noexcept -> size_t { return layout; }
auto Descriptor::Binding() const noexcept -> size_t {return binding; } 
auto Descriptor::ShaderStages() const noexcept -> vk::ShaderStageFlags { return shaderStages; } 
auto Descriptor::DescriptorType() const noexcept -> vk::DescriptorType { return type; } 
auto Descriptor::HasBuffer() const noexcept -> bool  { return buffer.get(); } 
auto Descriptor::HasImage() const noexcept -> bool { return image.get(); } 

} // namespace nj::ren
