#pragma once
#ifndef NJ_RENDER_SYNC_H
#define NJ_RENDER_SYNC_H

#include "nj_device.h"
#include <vulkan/vulkan_handles.hpp>

namespace nj::ren {

class SyncData {
  public:
    SyncData(ren::DeviceH device);
    virtual ~SyncData();

    vk::UniqueSemaphore availableSemaphore;
    vk::UniqueSemaphore finishSemaphore;
    vk::UniqueFence frameFence;
};
using SyncDataH = std::shared_ptr<SyncData>;

}; // namespace nj::ren

#endif
