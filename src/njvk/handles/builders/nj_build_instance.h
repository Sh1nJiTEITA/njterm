#pragma once
#ifndef NJ_BUILDER_INSTANCE_H
#define NJ_BUILDER_INSTANCE_H

#include "handles/nj_builder.h"
#include "njvkutils.h"
#include <set>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::build {

template <> class Builder<vk::Instance> {
  public:
    Builder(const std::set<std::string> &inext);
    Builder(const std::vector<std::string> &inext);

    std::set<std::string> inputExtensions;

    using Handle = vk::SharedInstance;
    Handle Build();

  private:
    auto FeaturesStruct() -> vk::ValidationFeaturesEXT;

  private:
    nj::ren::VarHandles h;
};

} // namespace nj::build
#endif
