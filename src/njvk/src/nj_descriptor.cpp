#include "nj_descriptor.h"
#include "njlog.h"
#include "njvkutils.h"
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

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


DescriptorBase::DescriptorBase(
    vk::ShaderStageFlags shader_stages, vk::DescriptorType desc_type
) 
    : shaderStages { shader_stages } 
    , descriptorType{ desc_type } 
{ 
}


DescriptorStatic::DescriptorStatic(
    vk::ShaderStageFlags shader_stages, vk::DescriptorType desc_type
) 
    : DescriptorBase(shader_stages, desc_type)
{
}

void DescriptorStatic::Initialize(DeviceH device, AllocatorH allocator) {
    CreateBuffer(device, allocator); 
    CreateImage(device, allocator);
    CreateView(device, allocator);
}

auto DescriptorStatic::GenBufferInfo() const -> vk::DescriptorBufferInfo{
    return vk::DescriptorBufferInfo{}
        .setBuffer(buffer->Handle())
        .setOffset(0)
        .setRange(buffer->InitialSize())
    ;
}

auto DescriptorStatic::GenImageInfo() const -> vk::DescriptorImageInfo{ 
    return vk::DescriptorImageInfo{}
        .setImageLayout(image->Layout())
        .setImageView(imageView->Handle())
    ;
}

// clang-format off

// clang-format on

void DescriptorStatic::FillWriteWithResourcesInfo(
    vk::WriteDescriptorSet& write,
    std::vector<vk::DescriptorBufferInfo>& buffer_infos,
    std::vector<vk::DescriptorImageInfo>& image_infos
) const {
    write.setDescriptorCount(1);

    if (buffer) {
        buffer_infos.push_back(GenBufferInfo());
        write.setBufferInfo(buffer_infos.back());
    }

    if (image && imageView) {
        image_infos.push_back(GenImageInfo());
        write.setImageInfo(image_infos.back());
    }
}

uint32_t DescriptorStatic::Count() const { return 1; }

bool DescriptorStatic::HasBuffer() const noexcept { return buffer.get(); }
bool DescriptorStatic::HasImage() const noexcept { return image.get(); }
bool DescriptorStatic::HasView() const noexcept { return imageView.get(); }

void* DescriptorStatic::MapBuffer() { return buffer->Map(); }
void DescriptorStatic::UnmapBuffer() { buffer->Unmap(); }

} // namespace nj::ren
