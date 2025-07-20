#pragma once
#ifndef NJ_VULKAN_OBJECT_H
#define NJ_VULKAN_OBJECT_H

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

template <typename T> class VulkanObject {
  public:
    VulkanObject() = default;
    using HandleType = T;

    vk::SharedHandle<T> &Handle() { return handle; }
    T CHandle() { return static_cast<typename T::CType>(handle.get()); }
    virtual std::string HandleName() const noexcept = 0;

  protected:
    vk::SharedHandle<T> handle;
};

template <typename T, typename Dispatch> class VulkanObjectUnique {
  public:
    VulkanObjectUnique() = default;
    using HandleType = T;

    T &Handle() { return handle; }
    virtual std::string HandleName() const noexcept = 0;

  protected:
    vk::UniqueHandle<T, Dispatch> handle;
};

}; // namespace nj::ren

#endif
