#include <memory>
#define VMA_IMPLEMENTATION

#include "nj_allocator.h"
#include "njvklog.h"

namespace nj::ren {

Allocator::Allocator(ren::InstanceH instance, ren::DeviceH device,
                     ren::PhysicalDeviceH phDevice) {

    VmaVulkanFunctions functions = {};

    functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo info = {};
    info.instance = instance->CHandle();
    info.device = device->CHandle();
    info.physicalDevice = phDevice->CHandle();
    info.pVulkanFunctions = &functions;
    info.vulkanApiVersion = VK_API_VERSION_1_3;

    VmaAllocator tmp;
    log::CheckCall((vk::Result)vmaCreateAllocator(&info, &tmp),
                   "Cant create Allocator");

    handle = std::make_shared<VmaAllocator>(tmp);
}

Allocator::~Allocator() { vmaDestroyAllocator(*handle.get()); }

std::shared_ptr<VmaAllocator> Allocator::Handle() { return handle; }
VmaAllocator *Allocator::CHandle() { return handle.get(); }

} // namespace nj::ren
