#pragma once
#ifndef NJ_VULKAN_SURFACE_H
#define NJ_VULKAN_SURCACE_H

#include "nj_handle.h"
#include "nj_instance.h"
#include <vulkan/vulkan_handles.hpp>

namespace nj::ren {
class Surface : public VulkanObjectNative<vk::SurfaceKHR> {
  public:
    Surface(InstanceH inst, vk::SurfaceKHR sur);
    virtual std::string HandleName() const noexcept override;
};
using SurfaceH = std::shared_ptr<Surface>;

}; // namespace nj::ren

#endif
