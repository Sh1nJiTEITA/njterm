#include "nj_debug_utils_messenger.h"
#include "njvklog.h"
#include <vulkan/vulkan_handles.hpp>

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

namespace nj::ren {

DebugUtilsMessenger::DebugUtilsMessenger(ren::InstanceH instance) {
    // clang-format off
    auto messenger_info = nj::log::VkLogCreateInfo();
    
    messenger_create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        instance->Handle().getProcAddr("vkCreateDebugUtilsMessengerEXT")
    );
    if (!messenger_create_func) { 
        log::FatalExit("Cant find create function for vk::DebugUtilsMessengerEXT. "
                       "Need to disable validation to work");
    }

    messenger_delete_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        instance->Handle().getProcAddr("vkDestroyDebugUtilsMessengerEXT")
    );

    if (!messenger_create_func) { 
        log::FatalExit("Cant find delete function for vk::DebugUtilsMessengerEXT. "
                       "Need to disable validation to work");
    }
    
    auto raw = instance->Handle().createDebugUtilsMessengerEXT(messenger_info);
    handle = vk::UniqueDebugUtilsMessengerEXT(raw, instance->Handle());
    // clang-format off
}

DebugUtilsMessenger::~DebugUtilsMessenger() { 

}



auto DebugUtilsMessenger::HandleName() const noexcept -> std::string {
    return "DebugUtilsMessenger";
}

} // namespace nj::ren
