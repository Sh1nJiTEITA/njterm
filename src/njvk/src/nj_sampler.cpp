#include "nj_sampler.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::ren {

Sampler::Sampler(DeviceH device) {

    vk::SamplerCreateInfo samplerInfo{};

    samplerInfo.magFilter = vk::Filter::eNearest;
    samplerInfo.minFilter = vk::Filter::eNearest;

    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

    // samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    // samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    // samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;

    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 4;

    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    handle = device->Handle().createSamplerUnique(samplerInfo);
}

std::string Sampler::HandleName() const noexcept { return "Sampler"; }

} // namespace nj::ren
