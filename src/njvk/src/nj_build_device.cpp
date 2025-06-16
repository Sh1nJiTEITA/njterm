#include "nj_build_device.h"
#include "nj_builder.h"
#include "njcon.h"
#include "njlog.h"
#include <algorithm>
#include <array>
#include <ranges>
#include <set>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace nj::build {

using BDevice = Builder<vk::Device>;

BDevice::Builder(ren::InstanceH instance, ren::PhysicalDeviceH phDevice,
                 vk::SharedSurfaceKHR surface)
    : inst{instance}, phDevice{phDevice}, surface{surface} {}

// clang-format off
BDevice::Handle BDevice::Build() {
    auto info = vk::DeviceCreateInfo{}
        // From builder 
        .setQueueCreateInfos( this->DeviceQueueInfos() )
        .setPEnabledExtensionNames( this->DeviceExtensions() )
        // From global settings
        .setPEnabledFeatures( &h.Handle(PhysicalDeviceFeatures()) )
        .setPNext( &h.Handle(PhysicalDeviceShaderFeatures()) )
    ;

    // NOTE: DEPRECATED IN NEW VULKAN VERSIONS
    // if (con::ValidationEnabled()) { 
    //     auto& str_layers = h.Handle(con::ValidationLayers());
    //     auto& layers = h.Handle(
    //         str_layers 
    //         | std::views::transform([](const std::string& str) { return str.c_str(); })
    //         | std::ranges::to<std::vector>()
    //     );
    //     info.setPEnabledLayerNames(layers);
    // }
    auto res = phDevice->Handle()->createDevice(info);
    return vk::SharedDevice(res);
}


const std::vector<vk::DeviceQueueCreateInfo> &BDevice::DeviceQueueInfos() { 
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
    
    auto& qinfos = h.Handle(std::vector<vk::DeviceQueueCreateInfo>{});
    for (size_t idx : unique_indices) { 
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
const std::vector<const char*>& BDevice::DeviceExtensions() { 
    const auto to_cstr_func = [](const std::string& f) { 
        return f.c_str();
    };
    const auto& str_features = h.Handle(DeviceFeatures());
    const auto& device_features = h.Handle(
        str_features
        | std::views::transform(to_cstr_func)
        | std::ranges::to<std::vector>()
    );
    return device_features;
}

// using SBDevice = Builder<ren::Device>;
//
// SBDevice::Builder(ren::InstanceH instance, ren::PhysicalDeviceH phDevice,
//                  vk::SharedSurfaceKHR surface)
//     : inst{instance}, phDevice{phDevice}, surface{surface} {}
//
// SBDevice::Handle SBDevice::Build() { 
//     auto device = std::make_shared<ren::Device>();
//     device->Handle() = Builder<vk::Device>(inst, phDevice, surface).Build();
//     return device;
// }
//


} // namespace nj::build
