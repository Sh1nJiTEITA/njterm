#pragma once
#include "nj_command_buffer.h"
#ifndef NJ_DESCRIPTOR_CONTEXT_H
#define NJ_DESCRIPTOR_CONTEXT_H

#include <nj_descriptor.h>

namespace nj::ren {

using DescriptorU = std::unique_ptr<Descriptor>;

// clang-format off
class DescriptorContext {
public:
    DescriptorContext(DeviceH device, DescriptorPoolH pool, AllocatorH allocator, size_t frames);
    ~DescriptorContext();
  
    void Add(size_t layout, size_t binding, std::vector<DescriptorU>&& descriptor);

    template <typename DescriptorType, typename ...Args>
    void Add(size_t frames, size_t layout, size_t binding, const Args& ...args) { 
        std::vector<DescriptorU> vec { };
        vec.reserve(frames);
        for (size_t frame_idx = 0; frame_idx < frames; ++frame_idx) { 
            vec.push_back(std::make_unique<DescriptorType>(std::forward<Args>(args)...));
        }
        Add(layout, binding, std::move(vec));
    }
    
    void CreateLayouts();
    void AllocateSets();
    void UpdateSets();
    void BindSets(size_t layout, size_t frame, CommandBufferH cb, vk::SharedPipelineLayout pllayout);
    auto AllLayouts() -> std::vector<vk::SharedDescriptorSetLayout>;
    auto Get(size_t frame, size_t layout, size_t binding) -> Descriptor&;
    
    template<typename T> 
    auto Get(size_t frame, size_t layout, size_t binding) -> T& { 
        return static_cast<T&>(Get(frame, layout, binding));
    };

  private:
    class Impl; std::unique_ptr<Impl> impl;
};
// clang-format on

} // namespace nj::ren

#endif
