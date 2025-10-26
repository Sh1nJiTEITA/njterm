#ifndef NJ_DESCRIPTOR_TEXTURE_ARRAY_H
#define NJ_DESCRIPTOR_TEXTURE_ARRAY_H

#include "nj_descriptor.h"
#include "nj_image_view.h"
#include "nj_sampler.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

struct DescriptorTextureArray : public DescriptorBase {
    DescriptorTextureArray(
        vk::ShaderStageFlags stages,
        std::vector<ImageViewU>&& image_views,
        std::vector<SamplerH>&& samplers
    );

    virtual void Initialize(DeviceH device, AllocatorH allocator) override;
    virtual void FillWriteWithResourcesInfo(
        vk::WriteDescriptorSet& write,
        std::vector<vk::DescriptorBufferInfo>& buffer_infos,
        std::vector<vk::DescriptorImageInfo>& image_infos
    ) const override;

protected:
    std::vector<ImageViewU> imageViews;
    std::vector<SamplerH> samplers;
};

}; // namespace nj::ren

#endif
