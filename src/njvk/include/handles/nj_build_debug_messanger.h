#pragma once
#ifndef NJ_BUILD_DEBUG_MESSANGER_H
#define NJ_BUILD_DEBUG_MESSANGER_H

#include "nj_builder.h"

namespace nj::build {

//! Creates unique ptr!
template <> class Builder<vk::DebugUtilsMessengerEXT> {
  public:
    Builder(vk::SharedInstance instance);

    using Handle = vk::SharedDebugUtilsMessengerEXT;
    Handle Build();

  private:
    vk::SharedInstance inst;
};

} // namespace nj::build

#endif
