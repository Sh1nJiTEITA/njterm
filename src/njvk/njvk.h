#pragma once
#ifndef NJ_VK_H
#define NJ_VK_H

#include <memory>
#include <vulkan/vulkan.hpp>

namespace nj {
namespace ren {

class Instance {

    Instance();
    ~Instance();

  protected:
    auto LayerProperties() -> std::vector<vk::LayerProperties>;

  private:
    std::unique_ptr<vk::Instance> instance;
};

} // namespace ren
} // namespace nj
#endif
