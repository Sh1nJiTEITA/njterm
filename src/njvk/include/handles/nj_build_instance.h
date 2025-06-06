#pragma once
#include <memory>
#ifndef NJ_BUILD_INSTANCE_H
#define NJ_BUILD_INSTANCE_H

#include "nj_builder.h"
#include "nj_instance.h"
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

template <> class Builder<ren::Instance> {
  public:
    Builder(const std::set<std::string> &inext);
    Builder(const std::vector<std::string> &inext);

    std::set<std::string> inputExtensions;

    using Handle = std::shared_ptr<ren::Instance>;
    Handle Build();
};

} // namespace nj::build
#endif
