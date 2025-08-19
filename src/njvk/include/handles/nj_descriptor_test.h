#pragma once
#ifndef NJ_DESCRIPTOR_TEST_H
#define NJ_DESCRIPTOR_TEST_H
#include "nj_descriptor.h"

namespace nj::ren {

// clang-format off



struct DescriptorTest : public ren::Descriptor {
    DescriptorTest(size_t layout, size_t binding);

    struct Data { 
        glm::vec3 color;
    };

    virtual void CreateBuffer(ren::DeviceH device, ren::AllocatorH allocator) override;
    virtual void CreateImage(ren::DeviceH device, ren::AllocatorH allocator) override;
    virtual void CreateView(ren::DeviceH device, ren::AllocatorH allocator) override;
};
// clang-format on

} // namespace nj::ren

#endif
