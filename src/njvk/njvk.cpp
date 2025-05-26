#include "njvk.h"
#include "njvklog.h"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>

namespace nj {
namespace ren {

auto Instance::LayerProperties() -> std::vector<vk::LayerProperties> {
    uint32_t layerCount;

    vk::Result res = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties");

    std::vector<vk::LayerProperties> layers(layerCount);
    res = vk::enumerateInstanceLayerProperties(&layerCount, layers.data());
    nj::log::CheckCall(res, "Cant enumerate vk instance layer properties 2");

    return layers;
}

} // namespace ren

} // namespace nj
