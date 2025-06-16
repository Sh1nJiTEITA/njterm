#pragma once
#ifndef NJ_VK_INSTANCE_H
#define NJ_VK_INSTANCE_H

#include "nj_handle.h"
#include <memory>
#include <set>
#include <vector>

namespace nj::ren {

class Instance : public VulkanObject<vk::Instance> {
  public:
    Instance(const std::set<std::string> &inext);
    Instance(const std::vector<std::string> &inext);
};

using InstanceH = std::shared_ptr<Instance>;

} // namespace nj::ren

#endif
