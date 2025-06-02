#include "nj_build_debug_messanger.h"
#include "njlog.h"
#include "njvklog.h"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

static PFN_vkCreateDebugUtilsMessengerEXT messenger_create_func;
static PFN_vkDestroyDebugUtilsMessengerEXT messenger_delete_func;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
    return messenger_create_func(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const *pAllocator) {
    return messenger_delete_func(instance, messenger, pAllocator);
}

namespace nj::build {

using BDebugMessenger = Builder<vk::DebugUtilsMessengerEXT>;

BDebugMessenger::Builder(vk::SharedInstance instance) : inst{instance} {}
BDebugMessenger::Handle BDebugMessenger::Build() {
    // clang-format off
    auto messenger_info = nj::log::VkLogCreateInfo();
    
    messenger_create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        inst->getProcAddr("vkCreateDebugUtilsMessengerEXT")
    );
    if (!messenger_create_func) { 
        nj::log::FatalExit("Cant find create function for vk::DebugUtilsMessengerEXT. "
                           "Need to disable validation to work");
    }

    messenger_delete_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        inst->getProcAddr("vkCreateDebugUtilsMessengerEXT")
    );

    if (!messenger_create_func) { 
        nj::log::FatalExit("Cant find delete function for vk::DebugUtilsMessengerEXT. "
                           "Need to disable validation to work");
    }
    
    auto raw = inst->createDebugUtilsMessengerEXTUnique(messenger_info);
    return vk::UniqueDebugUtilsMessengerEXT(std::move(raw));
    // clang-format off
}

} // namespace nj::build
