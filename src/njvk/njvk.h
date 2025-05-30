#pragma once
#ifndef NJ_VK_H
#define NJ_VK_H

#include <memory>
#include <vulkan/vulkan.hpp>

namespace nj {
namespace ren {

class Instance {

  public:
    Instance(const std::vector<std::string> &ext);
    ~Instance();

  private:
    std::unique_ptr<vk::Instance> instance;
};

auto AvailableLayers() -> std::vector<vk::LayerProperties>;
auto CheckLayerCompability() -> void;
auto AppInfo() -> vk::ApplicationInfo;

} // namespace ren
} // namespace nj
#endif
