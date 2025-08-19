#pragma once
#ifndef NJ_VULKAN_OBJECT_H
#define NJ_VULKAN_OBJECT_H

#include <memory>
#include <vulkan/vulkan.hpp>

namespace nj::ren {

// clang-format off

namespace meta { 
  template <typename T>
  using CaptureCType = typename std::remove_pointer_t<T>::CType;

  template <typename, typename = void>
  struct DetectCType { using type = void; };

  template <typename T>
  struct DetectCType<T, std::void_t<CaptureCType<T>>> { using type = CaptureCType<T>; };

  template <typename T>
  inline constexpr bool HasCType = !std::is_void_v<typename DetectCType<T>::type>;

  template <typename T>
  using ChainCheckVkHandle = std::conditional_t<HasCType<T>, typename DetectCType<T>::type, T*>;

  template <typename T>
  using ChainCheckPtr = std::conditional_t<std::is_pointer_v<T>, T, ChainCheckVkHandle<T>>;

  template <typename T>
  using CHandleType = ChainCheckPtr<T>;

}

template <typename T> class VulkanObjectInterface {
  public:
    VulkanObjectInterface() = default;
    using HandleType = T;
    using CHandleType = meta::CHandleType<T>;

    virtual HandleType &Handle() = 0;
    virtual CHandleType CHandle() = 0;
    virtual std::string HandleName() const noexcept = 0;
};

template <typename T> class VulkanObjectNative : public VulkanObjectInterface<T> {
  public:
    VulkanObjectNative() = default;
    using HandleType = T;
    using CHandleType = meta::CHandleType<T>;

    HandleType& Handle() override { return handle.get(); }
    CHandleType CHandle() override { return static_cast<CHandleType>(handle.get()); }

  protected:
    vk::UniqueHandle<T, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> handle;
};

template <typename T> class VulkanObjectManual : public VulkanObjectInterface<T> {
  public:
    VulkanObjectManual() = default;
    using HandleType = T;
    using CHandleType = meta::CHandleType<T>;

    HandleType& Handle() override { return *handle; }
    CHandleType CHandle() override { return static_cast<CHandleType>(*handle); }

  protected:
    std::unique_ptr<T> handle;
};

// clang-format on

}; // namespace nj::ren

#endif
