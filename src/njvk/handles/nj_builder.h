#pragma once
#ifndef NJ_BUILDER_H
#define NJ_BUILDER_H

#include <memory>
#include <njvkutils.h>
#include <vulkan/vulkan_shared.hpp>

namespace nj::build {

template <typename T> class Builder {
  public:
    using Handle = vk::SharedHandle<T>;
    Handle Build() { return vk::SharedHandle<T>(); }

  protected:
    ren::VarHandles h;
};

}; // namespace nj::build

#endif
