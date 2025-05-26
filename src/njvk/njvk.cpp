#include "njvk.h"
#include "njlog.h"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>

namespace nj {
namespace ren {

auto Instance::LayerProperties() -> std::vector<vk::LayerProperties> {
    uint32_t layerCount;
    vk::Result res;

    nj::log::CheckCall(
        vk::enumerateInstanceLayerProperties(&layerCount, nullptr), "");
    std::vector<VkLayerProperties> avaliableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());
}

} // namespace ren

} // namespace nj
