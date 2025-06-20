#include "nj_instance.h"
#include "nj_builder.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include "njvkutils.h"
#include <ranges>

namespace nj::ren {

auto FeaturesStruct(nj::ren::VarHandles &h) -> vk::ValidationFeaturesEXT {
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

Instance::Instance(const std::set<std::string> &inext) {
    // handle = build::Builder<vk::Instance>(inext).Build();
    nj::ren::VarHandles h;

    build::CheckVulkanCompability();
    std::set<std::string> inputExtensions(inext);
    std::vector<const char *> validation_layers{};
    auto transform_func = [](const std::string &s) { return s.c_str(); };

    // clang-format off
    if (con::ValidationEnabled()) {
        log::Info("Validation layers are enabled");
        for (auto& ext : build::AvailableValidationExtensions()) { 
            inputExtensions.insert(ext.extensionName);
        }
        for (auto& layer : con::ValidationLayers()) { 
            validation_layers.push_back(h.Handle(layer).c_str());
        }
    }
    auto ext = inputExtensions
        | std::views::transform(transform_func)
        | std::ranges::to<std::vector>()
    ;
    auto app_info = build::AppInfo();
    auto inst_info = vk::InstanceCreateInfo{}
        .setPEnabledExtensionNames(ext)
        .setPApplicationInfo(&app_info)
        .setPEnabledLayerNames(validation_layers)
        .setPNext(&h.Handle(FeaturesStruct(h)))
    ;
    auto raw = vk::createInstance(inst_info);
    handle = vk::SharedInstance(raw);
}
Instance::Instance(const std::vector<std::string> &inext) 
    : Instance{ inext | std::ranges::to<std::set>() } 
{
}

auto Instance::HandleName() const noexcept -> std::string { return "Instance"; }

} // namespace nj::ren
