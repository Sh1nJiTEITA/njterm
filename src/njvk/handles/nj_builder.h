#pragma once
#ifndef NJ_BUILDER_H
#define NJ_BUILDER_H

#include <njvkutils.h>
#include <set>
#include <vulkan/vulkan_shared.hpp>

namespace nj::build {

//! FIXME: Add explicit choice of physical device
template <typename T> class Builder {
  public:
    using Handle = vk::SharedHandle<T>;
    Handle Build() { return vk::SharedHandle<T>(); }

  protected:
    ren::VarHandles h;
};

template <typename T, typename... Args> decltype(auto) Build(Args &&...args) {
    return Builder<T>(std::forward<Args>(args)...).Build();
}

//! @defgroup InstanceSubFunc Subfunctions for instance creation
//! @{
auto AvailableLayers() -> std::vector<vk::LayerProperties>;
auto AvailableExtensions() -> std::vector<vk::ExtensionProperties>;
auto AvailableValidationExtensions() -> std::vector<vk::ExtensionProperties>;
auto CheckVulkanCompability() -> void;
auto AppInfo() -> vk::ApplicationInfo;
//! @}
//! @defgroup Subfunctions for device creation
//! @{

//! @}

}; // namespace nj::build

#endif
