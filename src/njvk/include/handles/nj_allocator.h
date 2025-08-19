#pragma once
#ifndef NJ_ALLOCATOR_H
#define NJ_ALLOCATOR_H

#include "nj_device.h"
#include "nj_physical_device.h"
#include <memory>
#include <vk_mem_alloc.h>

namespace nj::ren {

class Allocator : public VulkanObjectManual<VmaAllocator> {
  public:
    Allocator(ren::InstanceH instance, ren::DeviceH device,
              ren::PhysicalDeviceH phDevice);
    ~Allocator();

    virtual std::string HandleName() const noexcept;
};
using AllocatorH = std::shared_ptr<Allocator>;

class AllocationUnit {
  public:
    AllocationUnit(ren::AllocatorH allocator);
    auto Allocation() -> VmaAllocation &;
    auto Size() -> size_t;
    auto Id() -> uint32_t;

  protected:
    uint32_t id;
    ren::AllocatorH allocator;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;
};

} // namespace nj::ren

#endif
