#include "nj_build_instance.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include <ranges>

namespace nj::build {

using BInstance = Builder<vk::Instance>;

BInstance::Builder(const std::set<std::string> &inext)
    : inputExtensions{inext} {}

BInstance::Builder(const std::vector<std::string> &inext)
    : inputExtensions{inext | std::ranges::to<std::set>()} {}

BInstance::Handle BInstance::Build() {
    CheckVulkanCompability();
    auto inst_info = vk::InstanceCreateInfo{};
    auto transform_func = [](const std::string &s) { return s.c_str(); };
    // clang-format off
    if (nj::con::ValidationEnabled()) {
        nj::log::Info("Validation layers are enabled");
        
        for (auto& ext : AvailableValidationExtensions()) { 
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
    inst_info.setPApplicationInfo(&h.Handle(AppInfo()));
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

using SBInstance = Builder<ren::Instance>;

SBInstance::Builder(const std::set<std::string> &inext)
    : inputExtensions{inext} {}
SBInstance::Builder(const std::vector<std::string> &inext)
    : inputExtensions{inext | std::ranges::to<std::set>()} {}

SBInstance::Handle SBInstance::Build() {
    auto inst = std::make_shared<ren::Instance>();
    inst->Handle() = Builder<vk::Instance>(this->inputExtensions).Build();
    return inst;
}

} // namespace nj::build
