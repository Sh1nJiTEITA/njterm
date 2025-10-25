#pragma once
#ifndef NJ_DESCRIPTOR_CONTEXT_H
#define NJ_DESCRIPTOR_CONTEXT_H

#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_device.h"
#include "njlog.h"
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace nj::ren {

using DescriptorU = std::unique_ptr<Descriptor>;

namespace exp {

using DescriptorBaseU = std::unique_ptr<DescriptorBase>;

struct DescriptorSet {
    using BindingType = uint32_t;
    using FrameType = uint32_t;

    struct Pack {
        // Per frame
        const bool isSingle;
        const std::vector<DescriptorBaseU> handles;
    };

    //                                Ctors
    // ************************************************************************
    DescriptorSet(FrameType frames_count);
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) noexcept = default;
    DescriptorSet& operator=(DescriptorSet&&) noexcept = delete;

    //                            Pre allocation
    // ************************************************************************

    // clang-format off
    //! For per frame buffer/image descriptors
    template <typename DescriptorType, typename... Args>
    void RegisterPerFrame(BindingType binding, const Args&... args) {
        std::vector< DescriptorBaseU > tmp; tmp.reserve(framesCount);
        for (FrameType frame = 0; frame < framesCount; ++frame) { 
            tmp.emplace_back(std::make_unique< DescriptorType >(args...));
        }
        auto [_, success] = packs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(binding),
            std::forward_as_tuple(false, std::move(tmp))
        );
        log::FatalAssert(!success, "Binding={} occupied", binding);
    }

    //! For single buffer/image descriptors
    template <typename DescriptorType, typename... Args>
    void RegisterSingle(BindingType binding, Args&&...args) {
        std::vector< DescriptorBaseU > tmp;
        tmp.push_back(
            std::make_unique< DescriptorType >(std::forward<Args>(args)...)
        );
        auto [_, success] = packs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(binding),
            std::forward_as_tuple(true, std::move(tmp))
        );
        log::FatalAssert(!success, "Binding={} occupied", binding);
    }

    DescriptorBase& Get(FrameType frame, BindingType binding) {
        log::FatalAssert(!packs.contains(binding), "Binding={} was not registered", binding);
        const auto& pack = packs.at(binding);
        if (pack.isSingle) {
            log::FatalAssert(pack.handles.size()!=1, "Invalid count of descriptors for single one", binding);
            return *pack.handles.back();
        } else { 
            log::FatalAssert(pack.handles.size() < frame, "Frame={} > registered frames count", binding);
            return *pack.handles[frame];
        }
    }

    template <typename DescriptorType>
    DescriptorType& Get(FrameType frame, BindingType binding) {
        return static_cast<DescriptorType&>(Get(frame, binding));
    }
    // clang-format on

    vk::DescriptorSetLayout& LayoutHandle() noexcept;
    vk::DescriptorSet& Set(FrameType frame);

    std::vector<BindingType> RegisteredBindings() const;

    //                               Allocation
    // ************************************************************************

    void InitializeDescriptors(DeviceH device, AllocatorH allocator);

    virtual void Create(DeviceH device);

    //! Same as create vk::DesriptorSets
    virtual void Allocate(DeviceH device, DescriptorPoolH pool);

    //! Register existing inner descriptors for vulkan
    void Write(
        std::vector<vk::WriteDescriptorSet>& writes,
        std::vector<vk::DescriptorBufferInfo>& buffer_infos,
        std::vector<vk::DescriptorImageInfo>& image_infos
    );

private:
    //! Creates descriptor set layout. For simplicity every registered
    //! desccriptor are added to single layout.
    void InternalCreate(
        DeviceH device, const vk::DescriptorSetLayoutCreateFlags& flags,
        void* pnext = nullptr
    );
    void InternalAllocate(
        DeviceH device, DescriptorPoolH pool, void* pnext = nullptr
    );
    void AssertDescriptorCount() const;

private:
    //                                Consts
    // ************************************************************************
    const FrameType framesCount;

    //                                Fields
    // ************************************************************************
    std::unordered_map<BindingType, Pack> packs;
    std::vector<vk::UniqueDescriptorSet> vkSets;
    vk::UniqueDescriptorSetLayout layoutHandle;
};
using DescriptorSetU = std::unique_ptr<DescriptorSet>;
//
//
//
//
//
//
//
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

} // namespace exp
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
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
