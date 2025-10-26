#ifndef NJ_DESCRIPTOR_SET_H
#define NJ_DESCRIPTOR_SET_H

#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_device.h"
#include "njlog.h"
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace nj::ren {

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
    //! @note Does not set descriptor count -> it must be done inside each
    //! decriptor child classes
    void Write(
        std::vector<vk::WriteDescriptorSet>& writes,
        std::vector<vk::DescriptorBufferInfo>& buffer_infos,
        std::vector<vk::DescriptorImageInfo>& image_infos
    );

private:
    //! Creates descriptor set layout. For simplicity every registered
    //! desccriptor are added to single layout.
    void InternalCreate(
        DeviceH device,
        const vk::DescriptorSetLayoutCreateFlags& flags,
        void* pnext = nullptr
    );
    void InternalAllocate(
        DeviceH device,
        DescriptorPoolH pool,
        void* pnext = nullptr
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
} // namespace nj::ren

#endif
