#include "nj_device.h"
#include "nj_builder.h"
#include "njlog.h"
#include "njvkutils.h"
#include <ranges>

namespace nj::ren {

// clang-format off
const std::vector<vk::DeviceQueueCreateInfo> &
DeviceQueueInfos(ren::VarHandles& h, ren::PhysicalDeviceH phDevice) {
    // WARN: Until this moment phDevice (ren::PhysicalDevice) must be updated
    // So all needed indices must be already loaded
    // Default queue types
    const std::vector<vk::QueueFlags> needed_types = build::NeededQueueFamilyTypes();
    std::set<size_t> unique_indices;
    for (auto q : needed_types) {
        const size_t idx = phDevice->QueueIndex(q);
        unique_indices.insert(idx);
    }
    // Surface-present index
    const size_t present_idx = phDevice->PresentQueueIndex();
    unique_indices.insert(present_idx);

    auto &qinfos = h.Handle(std::vector<vk::DeviceQueueCreateInfo>{});
    for (size_t idx : unique_indices) {
        log::Debug("Unique queue family selected index={}", idx);
        auto qinf = vk::DeviceQueueCreateInfo{}
                        .setPQueuePriorities(&h.Handle(1.0f))
                        .setQueueCount(1)
                        .setQueueFamilyIndex(idx);
        qinfos.push_back(std::move(qinf));
    }
    return qinfos;
}

// FIXME: Unreadable? For now there are no understance of CORRECT
// way of using std::ranges/views ... But it seems pretty pleasent
const std::vector<const char *> &
DeviceExtensions(ren::VarHandles& h) {
    const auto to_cstr_func = [](const std::string &f) { return f.c_str(); };
    const auto &str_features = h.Handle(build::DeviceFeatures());
    const auto &device_features = h.Handle(str_features 
                                | std::views::transform(to_cstr_func) 
                                | std::ranges::to<std::vector>())
                                ;
    return device_features;
}

Device::Device(ren::InstanceH instance, ren::PhysicalDeviceH phDevice) {
    ren::VarHandles h;
    auto info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos( DeviceQueueInfos(h, phDevice) )
        .setPEnabledExtensionNames( DeviceExtensions(h) )
        // From global settings
        .setPEnabledFeatures( &h.Handle(build::PhysicalDeviceFeatures()) )
        .setPNext( &h.Handle(build::PhysicalDeviceShaderFeatures()) )
    ;

    auto res = phDevice->Handle()->createDevice(info);
    handle = vk::SharedDevice(res);
}

auto Device::HandleName() const noexcept -> std::string { return "Device"; }

} // namespace nj::ren
