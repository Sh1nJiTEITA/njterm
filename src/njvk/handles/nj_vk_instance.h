#pragma once
#ifndef NJ_VK_INSTANCE_H
#define NJ_VK_INSTANCE_H

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace nj::ren {

class Instance {

  public:
    Instance(const std::vector<const char *> &inext);
    ~Instance();

  private:
    std::shared_ptr<vk::Instance> instance;
};

} // namespace nj::ren

#endif
