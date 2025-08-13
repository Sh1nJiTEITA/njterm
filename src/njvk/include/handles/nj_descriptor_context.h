#pragma once
#ifndef NJ_DESCRIPTOR_CONTEXT_H
#define NJ_DESCRIPTOR_CONTEXT_H

#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "njlog.h"

namespace nj::ren {

using DescriptorU = std::unique_ptr<Descriptor>;

// clang-format off
class DescriptorContext {
public:
    DescriptorContext(DeviceH device, DescriptorPoolH pool, AllocatorH allocator, size_t frames);
    ~DescriptorContext();
  
    void Add(size_t layout, size_t binding, std::vector<DescriptorU>&& descriptor);

    // template <typename DescriptorType, typename ...Args>
    // void Add(size_t frames, size_t layout, size_t binding, const Args& ...args) { 
    //     log::Debug("Adding new descriptor to context; frames={}, layout={}, binding={}",
    //                frames, layout, binding);
    //     std::vector<DescriptorU> vec { };
    //     vec.reserve(frames);
    //     for (size_t frame_idx = 0; frame_idx < frames; ++frame_idx) { 
    //         vec.push_back(std::make_unique<DescriptorType>(
    //             layout, binding, std::forward<Args>(args)...)
    //         );
    //     }
    //     Add(layout, binding, std::move(vec));
    // }

    template <typename DescriptorType, typename ...Args>
    void Add(size_t frames, size_t layout, size_t binding, const Args& ...args) { 
        log::Debug("------------------------------------------------");
        log::Debug("Adding new descriptor to context: frames={}, layout={}, binding={}",
                   frames, layout, binding);
        std::vector<DescriptorU> vec { };
        vec.reserve(frames);
        for (size_t frame_idx = 0; frame_idx < frames; ++frame_idx) { 
            vec.push_back(std::make_unique<DescriptorType>(
                layout, binding, args...
            ));
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
