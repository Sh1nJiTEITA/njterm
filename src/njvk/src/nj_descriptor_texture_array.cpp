#include "nj_descriptor_texture_array.h"
#include "nj_descriptor.h"
#include "nj_log_scope.h"

namespace nj::ren {

DescriptorTextureArray::DescriptorTextureArray(
    vk::ShaderStageFlags stages,
    std::vector<ImageViewU>&& image_views,
    std::vector<SamplerH>&& samplers
)
    : DescriptorBase(stages, vk::DescriptorType::eCombinedImageSampler)
    , imageViews{std::move(image_views)}
    , samplers{std::move(samplers)} {}

void DescriptorTextureArray::Initialize(DeviceH device, AllocatorH allocator) {
    log::DebugA("Skipping initialization for DescriptorTextureArray");
}

void DescriptorTextureArray::FillWriteWithResourcesInfo(
    vk::WriteDescriptorSet& write,
    std::vector<vk::DescriptorBufferInfo>& buffer_infos,
    std::vector<vk::DescriptorImageInfo>& image_infos
) const {
    const size_t views_count = imageViews.size();

    if (image_infos.capacity() < views_count) {
        image_infos.reserve(image_infos.size() + views_count);
    }

    for (size_t i = 0; i < views_count; ++i) {
        auto& info = image_infos.emplace_back();
        info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        info.setImageView(imageViews[i]->Handle());
        info.setSampler(samplers[i]->Handle());
    }

    write.descriptorType = descriptorType;
    write.descriptorCount = static_cast<uint32_t>(views_count);
    write.setPImageInfo(
        image_infos.data() + (image_infos.size() - views_count)
    );
}

}; // namespace nj::ren
