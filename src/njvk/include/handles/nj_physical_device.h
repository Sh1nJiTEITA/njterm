#pragma once
#include <vulkan/vulkan_shared.hpp>
#ifndef NJ_PHYSICAL_DEVICE_H
#define NJ_PHYSICAL_DEVICE_H

#include "nj_handle.h"
#include "nj_instance.h"
#include <memory>
#include <optional>
#include <unordered_map>
#include <vulkan/vulkan_enums.hpp>

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
    auto PickFamilyIndex(vk::QueueFlagBits flag) -> std::optional<size_t>;

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
    auto UpdateQueues(vk::SharedDevice device) -> void;

    //! @return Found via UpdateQueueIndices PRESENT queue family index
    auto PresentQueueIndex() -> size_t;
    auto PresentQueue() -> vk::Queue&;
    auto GraphicsQueue() -> vk::Queue&;

    //! @return Found via UpdateQueueIndices <FLAG> queue family index
    auto QueueIndex(vk::QueueFlagBits flag) -> size_t;
    auto Queue(vk::QueueFlagBits flag) -> vk::Queue&;
    
    //! @return Unique family indices
    auto UniqueQueueIndices() -> std::vector<size_t>;
    auto HandleName() const noexcept -> std::string override;

  private:
    size_t presentIndex;  
    std::unordered_map<vk::QueueFlagBits, size_t> indices;
    std::unordered_map<vk::QueueFlagBits, vk::Queue> queues;
    vk::Queue presentQueue;

    //! Caching queue family properties
    std::vector<vk::QueueFamilyProperties> queueProperties;
};
using PhysicalDeviceH = std::shared_ptr<PhysicalDevice>;

}; // namespace nj::ren

#endif
