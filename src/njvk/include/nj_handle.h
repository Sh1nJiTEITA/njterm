#pragma once
#ifndef NJ_VULKAN_OBJECT_H
#define NJ_VULKAN_OBJECT_H

#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

template <typename T> class VulkanObject {
  public:
    VulkanObject() = default;

    vk::SharedHandle<T> &Handle() { return handle; }

  protected:
    vk::SharedHandle<T> handle;
};

}; // namespace nj::ren

#endif
