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


namespace exp { 


DescriptorBase::DescriptorBase(
    vk::ShaderStageFlags shader_stages, vk::DescriptorType desc_type
) 
    : shaderStages { shader_stages } 
    , descriptorType{ desc_type } 
{ }







};


Descriptor::~Descriptor() { }

auto Descriptor::LayoutBinding() -> vk::DescriptorSetLayoutBinding {
    size_t desc_count = 1;
    if (IsBindless()) {
        const size_t buf_sz = buffers.size();
        const size_t img_sz = images.size();
        const size_t vie_sz = imageViews.size();
        log::FatalAssert(
            buf_sz == img_sz == vie_sz,
            "buffers.size() ({}) " "!= images.size() ({}) " "!= imageViews.size() ({})", 
            buf_sz, img_sz, vie_sz
        );
        desc_count = buf_sz;
    }
    return vk::DescriptorSetLayoutBinding{}
        .setBinding(binding)
        .setDescriptorType(type)
        .setDescriptorCount(desc_count)
        .setStageFlags(shaderStages)
        ;
}

auto Descriptor::BufferInfo(size_t idx) -> vk::DescriptorBufferInfo {
    if (buffers.size() > idx) { 
        return  vk::DescriptorBufferInfo {}
            .setBuffer(buffers[idx]->CHandle())
            .setOffset(0)
            .setRange(buffers[idx]->InitialSize())
            ;
    }
    return {};
}

auto Descriptor::ImageInfo(size_t idx) -> vk::DescriptorImageInfo {
    if (images.size() > idx) { 
        return vk::DescriptorImageInfo{}
            .setImageView(imageViews[idx]->Handle())
            .setImageLayout(images[idx]->Layout())
            ;
    }
    return {};
}


auto Descriptor::IsBindless() -> bool {
    return bindless;
}

auto Descriptor::MapBuffer(size_t idx) -> void* { 
    if (!(buffers.size() > idx && buffers[idx].get()) ) { 
        log::FatalExit("Cant map descriptor buffer. Buffer has invalid handle");
    }
    return buffers[idx]->Map();
}

auto Descriptor::UnmapBuffer(size_t idx) -> void {
    if (!(buffers.size() > idx && buffers[idx].get()) ) { 
        log::FatalExit("Cant unmap descriptor buffer. Buffer has invalid handle");
    }
    buffers[idx]->Unmap();
}


auto Descriptor::Layout() const noexcept -> size_t { return layout; }
auto Descriptor::Binding() const noexcept -> size_t {return binding; } 
auto Descriptor::ShaderStages() const noexcept -> vk::ShaderStageFlags { return shaderStages; } 
auto Descriptor::DescriptorType() const noexcept -> vk::DescriptorType { return type; } 
auto Descriptor::HasBuffer() const noexcept -> bool  { return !buffers.empty(); } 
auto Descriptor::HasImage() const noexcept -> bool { return !images.empty(); } 

} // namespace nj::ren
