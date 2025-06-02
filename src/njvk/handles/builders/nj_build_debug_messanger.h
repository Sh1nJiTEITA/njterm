#pragma once
#ifndef NJ_BUILDER_DEBUG_MESSANGER_H
#define NJ_BUILDER_DEBUG_MESSANGER_H

#include "handles/nj_builder.h"
#include "njvkutils.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>

namespace nj::build {

//! Creates unique ptr!
template <> class Builder<vk::DebugUtilsMessengerEXT> {
  public:
    Builder(vk::SharedInstance instance);

    using Handle = vk::UniqueDebugUtilsMessengerEXT;
    Handle Build();

  private:
    vk::SharedInstance inst;
};

} // namespace nj::build

#endif
