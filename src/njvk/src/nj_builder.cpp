#include "nj_builder.h"
#include "nj_command_buffer.h"
#include "nj_physical_device.h"
#include "njcon.h"
#include "njvklog.h"
#include <ranges>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>

namespace nj::build {

auto AvailableLayers() -> std::vector<vk::LayerProperties> {
    uint32_t layerCount;

    vk::Result res = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties");

    std::vector<vk::LayerProperties> layers(layerCount);
    res = vk::enumerateInstanceLayerProperties(&layerCount, layers.data());
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties 2");

    return layers;
}

auto AvailableValidationExtensions() -> std::vector<vk::ExtensionProperties> {
    std::vector<vk::ExtensionProperties> available;

    const auto all_available = vk::enumerateInstanceExtensionProperties();
    auto required = con::ValidationExtensions()
                  | std::ranges::to<std::set<std::string_view>>();

    required.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    required.insert(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);

    for (const auto& extension : all_available) {
        if (required.contains(extension.extensionName)) {
            available.push_back(extension);
        }
    }

    return available;
}

auto CheckVulkanCompability() -> void {
    auto needed_l = con::ValidationLayers() | std::ranges::to<std::set>();
    auto avail_l = AvailableLayers();

    constexpr auto layer_msg = "{:<45} {:<12} {:<5}";
    nj::log::Info(layer_msg, "Layer name", "Version", "Is needed");
    for (auto& layer : avail_l) {
        const auto name = std::string(layer.layerName);
        const bool status = needed_l.contains(name);
        const std::string_view str_status = status ? "TRUE" : "FALSE";
        nj::log::Info(layer_msg, name, layer.specVersion, str_status);
        if (status) {
            needed_l.erase(name);
        }
    }
    if (!needed_l.empty()) {
        std::string msg{};
        for (auto& l : needed_l) {
            msg += l + ", ";
        }
        nj::log::FatalExit(
            "Program cant work... some layers are not available "
            "on this device. Needed layer list: {}",
            msg
        );
    }

    if (con::ValidationEnabled()) {
        // clang-format off
        auto needed_ext = con::ValidationExtensions() | std::ranges::to<std::set>();
        auto avail_ext = vk::enumerateInstanceExtensionProperties();
        // clang-format off
        const size_t init_ext_count = needed_ext.size(); 

        constexpr auto ext_msg = "{:<45} {:<12} {:<5}";
        nj::log::Info(ext_msg, "Extension name", "Version", "Is needed");
        for (auto &ext : avail_ext) {
            const auto name = std::string(ext.extensionName);
            const bool status = needed_ext.contains(name);
            const std::string_view str_status = status ? "TRUE" : "FALSE";
            nj::log::Info(ext_msg, name, ext.specVersion, str_status);
            if (status) {
                needed_ext.erase(name);
            }
        }

        std::string msg{};
        for (auto &l : needed_ext) msg += l + ", ";

        // Need to hardcode here because if vulkan debug is enabled 
        // and no below extesnions are provided there is now way to
        // debug
        // TODO: Rethink this approach (why should store validation 
        // extensions inside default config)
        const bool critical_ext_status = 1
            && needed_ext.contains("VK_EXT_debug_utils") 
            && needed_ext.contains("VK_EXT_validation_features");

        if (critical_ext_status) { 
            nj::log::FatalExit("Vulkan validation is enabled but no "
                               "validation extension are not present "
                               "for currect device. Needed extensions: {}", msg);
        } else if (!needed_ext.empty()) { 
            nj::log::Info("Not all wanted vulkan extensions "
                          "are present (its ok): {}", msg);
        } else { 
            nj::log::Info("All needed vulkan extensions are present");
        }
    }
}

auto AppInfo() -> vk::ApplicationInfo {
    // clang-format off
    vk::ApplicationInfo info;
    info.pApplicationName = APP_NAME;
    info.applicationVersion = VK_MAKE_VERSION(
        APP_VERSION_MAJOR, 
        APP_VERSION_MINOR, 
        APP_VERSION_PATCH
    );
    info.pEngineName = ENGINE_NAME;
    info.engineVersion = VK_MAKE_VERSION(
        ENGINE_VERSION_MAJOR, 
        ENGINE_VERSION_MINOR, 
        ENGINE_VERSION_PATCH
    );
    info.apiVersion = VK_API_VERSION_1_3;
    // clang-format on
    return info;
}

// clang-format off
// using OptQueue = std::optional<vk::QueueFamilyProperties>;
// auto PickFamilyIndex(vk::SharedPhysicalDevice phDevice) -> std::map<vk::QueueFlagBits, OptQueue> 
// { 
//     auto q = phDevice->getQueueFamilyProperties();
//     std::map<vk::QueueFlagBits, OptQueue> res { 
//         { vk::QueueFlagBits::eCompute,        std::nullopt },
//         { vk::QueueFlagBits::eGraphics,       std::nullopt },
//         { vk::QueueFlagBits::eOpticalFlowNV,  std::nullopt },
//         { vk::QueueFlagBits::eProtected,      std::nullopt },
//         { vk::QueueFlagBits::eSparseBinding,  std::nullopt },
//         { vk::QueueFlagBits::eTransfer,       std::nullopt },
//         { vk::QueueFlagBits::eVideoDecodeKHR, std::nullopt },
//         { vk::QueueFlagBits::eVideoEncodeKHR, std::nullopt },
//     };
//     
//     
//
//
//
// }

auto NeededQueueFamilyTypes() -> std::vector<vk::QueueFlagBits> { 
    return { 
        vk::QueueFlagBits::eGraphics,
        vk::QueueFlagBits::eTransfer
    };
}



auto PhysicalDeviceFeatures() -> vk::PhysicalDeviceFeatures { 
    return vk::PhysicalDeviceFeatures{}
        .setVertexPipelineStoresAndAtomics(true)
        .setFragmentStoresAndAtomics(true)
        .setIndependentBlend(true)
    ;
}

auto PhysicalDeviceShaderFeatures() -> vk::PhysicalDeviceShaderDrawParameterFeatures { 
    return  vk::PhysicalDeviceShaderDrawParameterFeatures{}
        .setShaderDrawParameters(true)
    ;
}

auto IndexingFeatures() -> vk::PhysicalDeviceDescriptorIndexingFeatures {
    return vk::PhysicalDeviceDescriptorIndexingFeatures{}
        .setDescriptorBindingPartiallyBound(true)
        .setDescriptorBindingVariableDescriptorCount(true)
        .setRuntimeDescriptorArray(true)
        ;
}

auto DeviceFeatures() -> std::vector<std::string> { 
    return { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}

auto CompositeAlpha(const vk::SurfaceCapabilitiesKHR& surface_cap) -> vk::CompositeAlphaFlagBitsKHR { 
    using bits = vk::CompositeAlphaFlagBitsKHR;
    vk::CompositeAlphaFlagsKHR f = surface_cap.supportedCompositeAlpha;

    if      ( f & bits::ePreMultiplied )  return bits::ePreMultiplied;
    else if ( f & bits::ePostMultiplied ) return bits::ePostMultiplied;
    else if ( f & bits::eInherit )        return bits::eInherit;
    else                                  return bits::eOpaque;
    
}

auto PickMinImageCount(const vk::SurfaceCapabilitiesKHR& surface_cap) -> uint32_t {
    return std::clamp(con::Buffering(), surface_cap.minImageCount, surface_cap.maxImageCount);
}

auto BeginCmdSingleCommand(vk::CommandBuffer& cmd) { 
    auto info = vk::CommandBufferBeginInfo{}.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    cmd.begin(info);
}


auto BeginCmdSingleCommand(ren::CommandBufferH cmd) -> void{ 
    auto info = vk::CommandBufferBeginInfo{}.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    cmd->Handle().begin(info);
}

auto EndCmdSingleCommand(ren::PhysicalDeviceH phDevice, ren::CommandBufferH cmd) -> void {
    cmd->Handle().end();
    auto command_buffers = std::array{cmd->Handle()};
    auto info = vk::SubmitInfo{}.setCommandBuffers(command_buffers);
    auto &queue = phDevice->GraphicsQueue();
    queue.submit(info, {});
    queue.waitIdle();
}

// clang-format on

} // namespace nj::build
