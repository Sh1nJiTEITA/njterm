
#include "nj_render_sync.h"
#include "njlog.h"
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

// clang-format off
SyncData::SyncData(ren::DeviceH device) {
    auto dev = device->Handle();
    availableSemaphore = vk::SharedSemaphore(dev->createSemaphore({}), dev);
    finishSemaphore = vk::SharedSemaphore(dev->createSemaphore({}), dev);
    
    auto fence_info = vk::FenceCreateInfo{}
        .setFlags(vk::FenceCreateFlagBits::eSignaled);

    frameFence = vk::SharedFence(dev->createFence(fence_info), dev);
}

SyncData::~SyncData() {
    log::Debug("Killing sync data..."); 
}
// clang-format on

} // namespace nj::ren
