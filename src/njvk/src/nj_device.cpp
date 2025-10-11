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
    const std::vector<vk::QueueFlagBits> needed_types = build::NeededQueueFamilyTypes();
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

    auto& physical_features = h.Handle(build::PhysicalDeviceFeatures());
    auto& physical_features2 = h.Handle(vk::PhysicalDeviceFeatures2{});
    auto& indexing_features = h.Handle(build::IndexingFeatures());
    auto& shader_features = h.Handle(build::PhysicalDeviceShaderFeatures());

    physical_features2.setFeatures(physical_features);
    physical_features2.setPNext(indexing_features);
    indexing_features.setPNext(shader_features);

    auto info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos( DeviceQueueInfos(h, phDevice) )
        .setPEnabledExtensionNames( DeviceExtensions(h) )
        // From global settings
        .setPNext( physical_features2 )
    ;

    handle = phDevice->Handle().createDeviceUnique(info);
}

auto Device::HandleName() const noexcept -> std::string { return "Device"; }

} // namespace nj::ren
