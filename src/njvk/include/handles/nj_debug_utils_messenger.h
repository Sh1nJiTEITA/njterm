#pragma once
#ifndef NJ_DEBUG_UTILS_MESSENGER_H
#define NJ_DEBUG_UTILS_MESSENGER_H

#include "nj_handle.h"
#include "nj_instance.h"
#include <memory>

namespace nj::ren {
// clang-format off
class DebugUtilsMessenger : public VulkanObjectNative<vk::DebugUtilsMessengerEXT> {
  public:
    DebugUtilsMessenger(ren::InstanceH instance);
    virtual ~DebugUtilsMessenger();
    auto HandleName() const noexcept -> std::string override;
};
// clang-format on
using DebugUtilsMessengerH = std::shared_ptr<DebugUtilsMessenger>;

} // namespace nj::ren

#endif
