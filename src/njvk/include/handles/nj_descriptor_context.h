#pragma once
#ifndef NJ_DESCRIPTOR_CONTEXT_H
#define NJ_DESCRIPTOR_CONTEXT_H

#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_descriptor_set.h"
#include "nj_device.h"
#include <memory>
#include <unordered_map>
#include <utility>

namespace nj::ren {

// clang-format off
class DescriptorContext {
public:
    using LayoutType = uint32_t;
    using FrameType = uint32_t;
    using BindingType = uint32_t;

    DescriptorContext() = default;
    DescriptorContext(const DescriptorContext&) = delete;
    DescriptorContext& operator=(const DescriptorContext&) = delete;
    DescriptorContext(DescriptorContext&&) noexcept = default;
    DescriptorContext& operator=(DescriptorContext&&) noexcept = delete;

    void Add(LayoutType layout, DescriptorSetU&& set);

    template < typename DescriptorSetType, typename ...Args >
    void Add(LayoutType layout, Args&& ...args) {
        Add(layout, std::make_unique< DescriptorSetType >(
            std::forward<Args>(args)...
        ));
    }

    DescriptorBase& GetDescriptor(LayoutType layout, BindingType binding, FrameType frame);
    
    template <typename DescriptorType> 
    DescriptorType& GetDescriptor(LayoutType layout, BindingType binding, FrameType frame) {
        return static_cast<DescriptorType&>(GetDescriptor(layout, binding, frame));
    }

    DescriptorSet& GetSet(LayoutType layout); 

    void Create(DeviceH device); 
    void Allocate(DeviceH device, DescriptorPoolH pool);
    void Update(DeviceH device);
    void Bind(LayoutType layout, FrameType frame, CommandBufferH cmd, 
              vk::PipelineLayout pipeline_layout);
    std::vector<vk::DescriptorSetLayout> AllLayouts();

private:
    std::unordered_map<LayoutType, DescriptorSetU> sets;
};

// clang-format on

} // namespace nj::ren

#endif
