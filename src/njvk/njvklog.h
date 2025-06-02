#pragma once

#ifndef NJ_VK_LOG_H
#define NJ_VK_LOG_H

#include "njlog.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace nj::log {
template <typename... Args>
inline auto CheckCall(vk::Result res, fmt::format_string<Args...> str,
                      Args &&...args) {
    if (res != vk::Result::eSuccess) {
        log::Fatal(str, std::forward<Args>(args)...);
        fmt::print(stderr, "Vulkan info: {}", vk::to_string(res));
        fmt::print(stderr, "System info: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }
}

static auto VkLog(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  vk::DebugUtilsMessageTypeFlagsEXT messageType,
                  const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData) -> unsigned int {
    switch (messageSeverity) {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        Info("[VK] {}", pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        Debug("[VK] {}", pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        Warn("[VK] {}", pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        Error("[VK] {}", pCallbackData->pMessage);
        break;
    }
    return VK_FALSE;
}

static auto VkLogCreateInfo() -> vk::DebugUtilsMessengerCreateInfoEXT {
    // clang-format off
    vk::DebugUtilsMessengerCreateInfoEXT info{};
    info.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo 
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose 
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         ;
    info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                     ;
    info.pfnUserCallback = VkLog;
    return info;
    // clang-format on
};

} // namespace nj::log

#endif
