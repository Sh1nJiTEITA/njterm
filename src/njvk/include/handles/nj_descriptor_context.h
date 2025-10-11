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
  

    void RegisterLayout(size_t layout, const vk::DescriptorSetLayoutCreateInfo& info,
                                       const vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT& var_info);

    void Add(size_t layout, size_t binding, std::vector<DescriptorU>&& descriptor);

    template <typename DescriptorType, typename ...Args>
    void Add(size_t frames, size_t layout, size_t binding, Args&& ...args) { 
        log::Debug("------------------------------------------------");
        log::Debug("[+] Adding descriptor(s): frames={}, layout={}, binding={}",
                   frames, layout, binding);

        std::vector<DescriptorU> vec;
        vec.reserve(frames);

        constexpr bool any_move_only = !(
            std::is_copy_constructible_v<std::decay_t<Args>> && ...
        );

        if constexpr (any_move_only) {
            log::FatalAssert(frames > 1, "Cannot create multiple descriptors "
                                         "with move-only arguments.");
        }

        for (size_t i = 0; i < frames; ++i) {
            if constexpr (any_move_only) {
                vec.push_back(std::make_unique<DescriptorType>(
                    layout, binding, std::forward<Args>(args)...\
                ));
                break;
            } else {
                vec.push_back(std::make_unique<DescriptorType>(
                    layout, binding, args...
                ));
            }
        }

        Add(layout, binding, std::move(vec));
    }

    void CreateLayouts();
    void AllocateSets();
    void UpdateSets();
    void BindSets(size_t layout, size_t frame, CommandBufferH cb, vk::PipelineLayout pllayout);
    auto AllLayouts() -> std::vector<vk::DescriptorSetLayout>;
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
