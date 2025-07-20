#pragma once
#include "nj_physical_device.h"
#include <memory>
#ifndef NJ_ALLOCATOR_H
#define NJ_ALLOCATOR_H

#include "nj_device.h"
#include <vk_mem_alloc.h>

namespace nj::ren {

class Allocator {
  public:
    Allocator(ren::InstanceH instance, ren::DeviceH device,
              ren::PhysicalDeviceH phDevice);
    ~Allocator();

    std::shared_ptr<VmaAllocator> Handle();
    VmaAllocator *CHandle();

  private:
    std::shared_ptr<VmaAllocator> handle;
};
using AllocatorH = std::shared_ptr<Allocator>;

} // namespace nj::ren

#endif
