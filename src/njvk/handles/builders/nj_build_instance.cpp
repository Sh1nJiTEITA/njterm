#include "nj_build_instance.h"
#include "handles/nj_vk_instance.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include <ranges>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::build {

using BInstance = Builder<vk::Instance>;

BInstance::Builder(const std::set<std::string> &inext)
    : inputExtensions{inext} {}

BInstance::Handle BInstance::Build() {
    ren::CheckVulkanCompability();
    auto inst_info = vk::InstanceCreateInfo{};
    auto transform_func = [](const std::string &s) { return s.c_str(); };
    // clang-format off
    if (nj::con::ValidationEnabled()) {
        nj::log::Info("Validation layers are enabled");
        
        for (auto& ext : ren::AvailableValidationExtensions()) { 
            inputExtensions.insert(ext.extensionName);
        }

        auto& validation_layers = h.Handle(
            con::ValidationLayers()
            | std::views::transform(transform_func) 
            | std::ranges::to<std::vector>()
        );    
    }
    auto& ext = h.Handle( 
        inputExtensions
        | std::views::transform(transform_func)
        | std::ranges::to<std::vector>()
    );
    inst_info.setPEnabledExtensionNames(ext);
    inst_info.setPApplicationInfo(&h.Handle(ren::AppInfo()));
    inst_info.setPNext(&h.Handle(FeaturesStruct()));
    auto raw = vk::createInstance(inst_info);
    return vk::SharedInstance(raw);
    // clang-format on
}

auto BInstance::FeaturesStruct() -> vk::ValidationFeaturesEXT {
    // clang-format off
    const auto convert = [](int ext) { 
        return static_cast<vk::ValidationFeatureEnableEXT>(ext);     
    };
    auto& validation_features = h.Handle(
        con::ValidationFeatures() 
        | std::views::transform(convert)
        | std::ranges::to<std::vector>()
    );

    auto& debug_info = h.Handle(nj::log::VkLogCreateInfo());
    auto g = vk::ValidationFeaturesEXT{}
        .setEnabledValidationFeatures(validation_features)
        .setPNext(&debug_info)
    ;
    return g;
    // clang-format on
}

} // namespace nj::build
