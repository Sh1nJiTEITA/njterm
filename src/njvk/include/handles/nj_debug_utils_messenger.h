#pragma once
#ifndef NJ_DEBUG_UTILS_MESSENGER_H
#define NJ_DEBUG_UTILS_MESSENGER_H

#include "nj_handle.h"
#include "nj_instance.h"
#include <memory>

namespace nj::ren {

class DebugUtilsMessenger : public VulkanObject<vk::DebugUtilsMessengerEXT> {
  public:
    DebugUtilsMessenger(ren::InstanceH instance);
    auto HandleName() const noexcept -> std::string override;
};

using InstanceH = std::shared_ptr<Instance>;

} // namespace nj::ren

#endif
