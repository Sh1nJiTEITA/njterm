#pragma once
#ifndef NJ_VULKAN_IMAGE_VIEW_H
#define NJ_VULKAN_IMAGE_VIEW_H

#include "nj_device.h"
#include "nj_handle.h"

namespace nj::ren {

class ImageView : public VulkanObjectNative<vk::ImageView> {
public:
    ImageView(vk::UniqueImageView&& image);
    auto HandleName() const noexcept -> std::string override;
};
using ImageViewH = std::shared_ptr<ImageView>;
using ImageViewU = std::unique_ptr<ImageView>;

} // namespace nj::ren

#endif
