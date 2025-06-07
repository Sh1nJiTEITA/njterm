#pragma once
#ifndef NJ_BUILDER_H
#define NJ_BUILDER_H

#include <map>
#include <njvkutils.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::build {
// clang-format off
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

//! @defgroup vk-settings
//! Group for redefining vk-constants

//! @defgroup InstanceSubFunc Subfunctions for instance creation
//! NOTE: Some vars are stored initially inside config (bad design maybe?)
//! @{
auto AvailableLayers() -> std::vector<vk::LayerProperties>;
auto AvailableExtensions() -> std::vector<vk::ExtensionProperties>;
auto AvailableValidationExtensions() -> std::vector<vk::ExtensionProperties>;
auto CheckVulkanCompability() -> void;
auto AppInfo() -> vk::ApplicationInfo;
//! @}


//! @defgroup Subfunctions for device creation
//! @{

//! Searches needed (by flag-bit value) comp queue-family index
//! @param prop Queue family indices to search among
//! @param flag Bit to find index for
auto PickFamilyIndex(const std::vector<vk::QueueFamilyProperties>& prop, vk::QueueFlags flag) -> std::optional<size_t>;

//! Searches queue family index comp with SurfaceKHR a.k.a PRESENT queue
//! @param phDevice Physical device pointer to check comp
//! @param surface Surface to check comp with
auto PickSurfaceFamilyIndex(vk::SharedPhysicalDevice phDevice, vk::SharedSurfaceKHR surface) -> std::optional<size_t>;

//! @ingroup vk-settings
//! Needed queue families are physical device "items?", smt which can make some work
//! Each queue can do multiple types of jobs like computing & presenting images 
//! on screen
//! This funcs returns needed to app queue-types
//!  FIXME: For some reason it returns vec of bits but
//!  it is BAD DESIGN
auto NeededQueueFamilyTypes() -> std::vector<vk::QueueFlags>;

auto NeededQueueIndices(vk::SharedPhysicalDevice phDevice, vk::SharedSurfaceKHR surface) -> std::map<vk::QueueFlags, size_t>;
auto IsPresentQueueSeparate(std::map<vk::QueueFlags, size_t> map, 
                            vk::SharedPhysicalDevice phDevice, 
                            vk::SharedSurfaceKHR surface) -> bool;

//! @ingroup vk-settings
//! Physical device features are stored here, not inside config (bad design maybe?)
//! To override / add new features -> modify this func
auto PhysicalDeviceFeatures() -> vk::PhysicalDeviceFeatures;

//! @ingroup vk-settings
//! ...
auto PhysicalDeviceShaderFeatures() -> vk::PhysicalDeviceShaderDrawParameterFeatures;

//! @ingroup vk-settings
//! Stores vk-device needed features needed to run the program
//! At least func returns swap-chain feature
auto DeviceFeatures() -> std::vector<std::string>;
//! @}


//! @}
//! @defgroup Subfunctions for swapchain creation
//! @{

//! Finds out what alpha-regime is most capable for current system setup
//! This connected mostly with window transparency, if it is present in
//! current OS
auto CompositeAlpha(const vk::SurfaceCapabilitiesKHR& surface_cap) -> vk::CompositeAlphaFlagBitsKHR;


//! Finds minimal image count to render (buffirization)
//! Default buffering is TRIPLE buffering, but can be changed from config
auto MinImageCount(const vk::SurfaceCapabilitiesKHR& surface_cap) -> uint32_t;





//! @}

// clang-format on
}; // namespace nj::build

#endif
