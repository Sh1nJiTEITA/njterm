#include "njvk.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>

namespace nj {
namespace ren {

Instance::Instance(const std::vector<std::string> &inext) {
    CheckLayerCompability();
    auto ext = inext;
    if (con::ValidationEnabled()) {
        ext.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        ext.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
    }
    auto app_info = AppInfo();
}

Instance::~Instance() {}

auto AvailableLayers() -> std::vector<vk::LayerProperties> {
    uint32_t layerCount;

    vk::Result res = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties");

    std::vector<vk::LayerProperties> layers(layerCount);
    res = vk::enumerateInstanceLayerProperties(&layerCount, layers.data());
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties 2");

    return layers;
}

auto CheckLayerCompability() -> void {
    auto needed_l = con::ValidationLayersSet();
    auto avail_l = AvailableLayers();

    constexpr auto msg = "{:<45} {:<12} {:<5}";
    nj::log::Info(msg, "Layer name", "Version", "Is needed");
    for (auto &layer : avail_l) {
        const auto name = std::string(layer.layerName);
        const bool status = needed_l.contains(name);
        const std::string_view str_status = status ? "TRUE" : "FALSE";
        nj::log::Info(msg, name, layer.specVersion, str_status);
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

} // namespace ren

} // namespace nj
