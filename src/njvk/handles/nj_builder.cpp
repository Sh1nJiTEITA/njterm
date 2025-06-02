#include "nj_builder.h"
#include "njcon.h"
#include "njvklog.h"
#include <ranges>

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
    auto required = con::ValidationExtensions() |
                    std::ranges::to<std::set<std::string_view>>();

    required.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    required.insert(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);

    for (const auto &extension : all_available) {
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
    for (auto &layer : avail_l) {
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
        for (auto &l : needed_l) {
            msg += l + ", ";
        }
        nj::log::FatalExit("Program cant work... some layers are not available "
                           "on this device. Needed layer list: {}",
                           msg);
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

} // namespace nj::build
