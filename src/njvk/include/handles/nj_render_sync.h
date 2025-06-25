#pragma once
#ifndef NJ_RENDER_SYNC_H
#define NJ_RENDER_SYNC_H

#include "nj_device.h"
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

class SyncData {

  public:
    SyncData(ren::DeviceH device);

    vk::SharedSemaphore availableSemaphore;
    vk::SharedSemaphore finishSemaphore;
    vk::SharedFence frameFence;
};
using SyncDataH = std::shared_ptr<SyncData>;

}; // namespace nj::ren

#endif
