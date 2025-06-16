#pragma once
#ifndef NJ_PHYSICAL_DEVICE_H
#define NJ_PHYSICAL_DEVICE_H

#include "nj_handle.h"
#include "nj_instance.h"
#include <map>
#include <memory>
#include <optional>

namespace nj::ren {

// clang-format off
class PhysicalDevice : public VulkanObject<vk::PhysicalDevice> {
  public:
    PhysicalDevice(ren::InstanceH instance);

    //! Updates inner queue properties if inner storage is empty
    //! Assuming that getting queue properties is not cheap operation
    //! Caching results.
    auto UpdateQueueProperties() -> void;

    //! Searches needed (by flag-bit value) comp queue-family index
    //! Not caching results.
    //! @param flag Flag (type) of queue to search for
    //! @return Possiable index of queue if it is found
    auto PickFamilyIndex(vk::QueueFlags flag) -> std::optional<size_t>;

    //! Searches queue family index comp with SurfaceKHR a.k.a PRESENT queue
    //! Not caching results.
    //! @param phDevice Physical device pointer to check comp
    //! @param surface Surface to check comp with
    auto PickSurfaceFamilyIndex(vk::SharedSurfaceKHR surface) -> std::optional<size_t>;

    //! Updates and caches needed (from config) queue family indices
    //! WARN: Must be called before using this class for ren::Instance handle
    //! creation
    //! @param surface Surface handle to find present family index for
    auto UpdateQueueIndices(vk::SharedSurfaceKHR surface) -> void; 

    //! @return Found via UpdateQueueIndices PRESENT queue family index
    auto PresentQueueIndex() -> size_t;

    //! @return Found via UpdateQueueIndices <FLAG> queue family index
    auto QueueIndex(vk::QueueFlags flag) -> size_t;
    
    //! @return Unique family indices
    auto UniqueQueueIndices() -> std::vector<size_t>;

  private:
    size_t presentIndex;  
    std::map<vk::QueueFlags, size_t> indices;

    //! Caching queue family properties
    std::vector<vk::QueueFamilyProperties> queueProperties;
};
using PhysicalDeviceH = std::shared_ptr<PhysicalDevice>;

}; // namespace nj::ren

#endif
