#pragma once
#ifndef NJ_VK_INSTANCE_H
#define NJ_VK_INSTANCE_H

#include "nj_handle.h"
#include <memory>

namespace nj::ren {

class Instance : public VulkanObject<vk::Instance> {
  public:
    Instance() = default;
};

using InstanceH = std::shared_ptr<Instance>;

} // namespace nj::ren

#endif
