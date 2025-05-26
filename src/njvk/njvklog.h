#pragma once

#ifndef NJ_VK_LOG_H
#define NJ_VK_LOG_H

#include "njlog.h"
#include <vulkan/vulkan.hpp>

namespace nj::log {
template <typename... Args>
inline auto CheckCall(vk::Result res, Args &&...args) {
    if (res != vk::Result::eSuccess) {
        log::Fatal(std::forward<Args>(args)...);
        fmt::print(stderr, "Vulkan info: {}", vk::to_string(res));
        fmt::print(stderr, "System info: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nj::log

#endif
