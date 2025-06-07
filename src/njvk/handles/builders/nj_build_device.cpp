#include "nj_build_device.h"
#include "handles/nj_builder.h"
#include "njcon.h"
#include "njlog.h"
#include <array>
#include <ranges>
#include <set>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace nj::build {

using BDevice = Builder<vk::Device>;

BDevice::Builder(vk::SharedInstance instance, vk::SharedPhysicalDevice phDevice,
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
    auto res = phDevice->createDevice(info);
    return vk::SharedDevice(res);
}


const std::vector<vk::DeviceQueueCreateInfo> &BDevice::DeviceQueueInfos() { 
    auto props = phDevice->getQueueFamilyProperties();
    // NOTE:
    // HARDCODED FOR NOW 
    // const auto flags = NeededQueueFamilyTypes();
    // // Searching for default vulkan queue-incides
    // std::set<size_t> indices;
    // log::Info("Searching for queue-family-indices...");
    // for (auto flag : flags) { 
    //     const auto index = PickFamilyIndex(props, flag);
    //     if (!index.has_value()) { 
    //         nj::log::FatalExit("Cant find queue family index for {}. "
    //                            "Videocard is not compatible",
    //                            vk::to_string(flag));
    //     }
    //     log::Info("Family with index={} is compatible with queue-type={}", index.value(), vk::to_string(flag));
    //     indices.insert(index.value());
    // }
    
    std::map<vk::QueueFlags, size_t> indices = NeededQueueIndices(phDevice, surface);

    // Searching now for present KHR queue
    const auto present_index = PickSurfaceFamilyIndex(phDevice, surface);
    if (!present_index.has_value()) { 
        nj::log::FatalExit("Cant find queue family index for KHR present logic. "
                           "Videocard is not compatible");
    }
    indices.insert(present_index.value());
    
    auto& qinfos = h.Handle(std::vector<vk::DeviceQueueCreateInfo>{});
    for (size_t idx : indices) { 
        auto qinf = vk::DeviceQueueCreateInfo{}
            .setPQueuePriorities(&h.Handle(1.0f))
            .setQueueCount(1)
            .setQueueFamilyIndex(idx);
        qinfos.push_back(std::move(qinf));
    }
    return qinfos;
}

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

} // namespace nj::build
