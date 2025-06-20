#include "nj_physical_device.h"
#include "nj_build_physical_device.h"
#include "nj_builder.h"
#include "njlog.h"
#include <ranges>
#include <set>

namespace nj::ren {

PhysicalDevice::PhysicalDevice(ren::InstanceH instance) {
    handle = build::Build<vk::PhysicalDevice>(instance->Handle());
}

auto PhysicalDevice::UpdateQueueProperties() -> void {
    if (queueProperties.empty()) {
        queueProperties = handle->getQueueFamilyProperties();
    }
}

auto PhysicalDevice::PickFamilyIndex(vk::QueueFlags flag)
    -> std::optional<size_t> {
    UpdateQueueProperties();
    const auto hasFlag = [flag](const vk::QueueFamilyProperties &f) {
        return static_cast<bool>(f.queueFlags & flag);
    };
    const auto it = std::ranges::find_if(queueProperties, hasFlag);
    if (it == queueProperties.end()) {
        return std::nullopt;
    }
    return std::distance(queueProperties.begin(), it);
}

auto PhysicalDevice::PickSurfaceFamilyIndex(vk::SharedSurfaceKHR surface)
    -> std::optional<size_t> {
    UpdateQueueProperties();
    for (size_t idx = 0; idx < queueProperties.size(); ++idx) {
        const bool has_support = handle->getSurfaceSupportKHR(idx, *surface);
        if (has_support) {
            return idx;
        }
    }
    return std::nullopt;
}

auto PhysicalDevice::UpdateQueueIndices(vk::SharedSurfaceKHR surface) -> void {
    // Getting needed queue-types from config
    std::vector<vk::QueueFlags> needed = build::NeededQueueFamilyTypes();
    log::Info("Searching for queue-family-indices...");
    for (auto flag : needed) {
        const auto index = PickFamilyIndex(flag);
        if (!index.has_value()) {
            nj::log::FatalExit("Cant find queue family index for {}. "
                               "Videocard is not compatible",
                               vk::to_string(flag));
        }
        log::Info("Family with index={} is compatible with queue-type={}",
                  index.value(), vk::to_string(flag));
        indices[flag] = index.value();
    }
    const auto present_idx = PickSurfaceFamilyIndex(surface);
    if (!present_idx.has_value()) {
        nj::log::FatalExit(
            "Cant find queue family index for KHR present logic. "
            "Videocard is not compatible");
    }
}

//! @return Found via UpdateQueueIndices PRESENT queue family index
auto PhysicalDevice::PresentQueueIndex() -> size_t { return presentIndex; }

//! @return Found via UpdateQueueIndices <FLAG> queue family index
auto PhysicalDevice::QueueIndex(vk::QueueFlags flag) -> size_t {
    return indices.at(flag);
}

auto PhysicalDevice::UniqueQueueIndices() -> std::vector<size_t> {
    std::set<size_t> set;
    for (auto &[type, idx] : indices) {
        set.insert(idx);
    }
    set.insert(PresentQueueIndex());
    return set | std::ranges::to<std::vector>();
}

auto PhysicalDevice::HandleName() const noexcept -> std::string {
    return "PhysicalDevice";
}

} // namespace nj::ren
