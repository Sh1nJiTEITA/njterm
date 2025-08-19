#include <memory>
#define VMA_IMPLEMENTATION

#include "nj_allocator.h"
#include "njvklog.h"
#include "njvkutils.h"

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

    handle = std::make_unique<VmaAllocator>(tmp);
}

Allocator::~Allocator() { vmaDestroyAllocator(*handle.get()); }
std::string Allocator::HandleName() const noexcept { return "Allocator"; }

AllocationUnit::AllocationUnit(ren::AllocatorH allocator)
    : allocator{allocator}, id{GenerateId()} {}
auto AllocationUnit::Allocation() -> VmaAllocation & { return allocation; }
auto AllocationUnit::Size() -> size_t { return allocationInfo.size; }
auto AllocationUnit::Id() -> uint32_t { return id; }

} // namespace nj::ren
