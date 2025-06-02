#pragma once
#ifndef NJ_VK_INSTANCE_H
#define NJ_VK_INSTANCE_H

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nj::ren {

class Instance {

  public:
    Instance(const std::vector<const char *> &inext);
    ~Instance();

  private:
    std::shared_ptr<vk::Instance> instance;
};

//! @defgroup InstanceSubFunc Subfunctions for instance creation
//! @{
auto AvailableLayers() -> std::vector<vk::LayerProperties>;
auto AvailableExtensions() -> std::vector<vk::ExtensionProperties>;
auto AvailableValidationExtensions() -> std::vector<vk::ExtensionProperties>;
auto CheckVulkanCompability() -> void;
auto AppInfo() -> vk::ApplicationInfo;
//! @}

} // namespace nj::ren

#endif
