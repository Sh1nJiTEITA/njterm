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

    //! Each pack is representation of single-typed descriptor
    struct Pack {
        //! Array descriptor mark
        const bool isArray;

        //! Single descriptor mark
        const bool isSingle;

        //! Stores handles
        //! If handle  > 1 then Descriptors were allocated per frame
        //! if handle == 1 then Descripotrs were allocated as single instance
        const std::vector<DescriptorBaseU> handles;
    };

    //                                Ctors
    // ************************************************************************

    //! Default ctor for descriptor set abstraction.
    //! As each descriptor set works directly with rendering loop so number of
    //! descriptors must be the same as frames count if descriptors are not
    //! heavy so \ref ::RegisterPerFrame might be used. If descriptor can be big
    //! and having mulitple instances per frame is a bad idea then \ref
    //! ::RegisterSingle might be used
    //! @param frames_count Number of rendering frames in flight
    DescriptorSet(FrameType frames_count);
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) noexcept = default;
    DescriptorSet& operator=(DescriptorSet&&) noexcept = delete;

    //                            Pre allocation
    // ************************************************************************

    // clang-format off
    //! For per frame buffer/image descriptors
    //! @tparam DescriptorType Descriptor type inherited from \ref ::DescriptorBase
    //! @param binding Bingind or coordinate inside set for shaders
    //! @param args L-value (for copy) arguments to construct descriptor of type \ref ::DescriptorType
    template <typename DescriptorType, typename... Args>
    void RegisterPerFrame(BindingType binding, const Args&... args) {
        std::vector< DescriptorBaseU > tmp; tmp.reserve(framesCount);
        for (FrameType frame = 0; frame < framesCount; ++frame) { 
            tmp.emplace_back(std::make_unique< DescriptorType >(args...));
        }
        auto [_, success] = packs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(binding),
            std::forward_as_tuple(
                /* isArray  */ false,
                /* isSingle */ false, 
                std::move(tmp)
            )
        );
        log::FatalAssertNot(!success, "Binding={} occupied", binding);
    }

    //! For single buffer/image descriptors
    //! @tparam DescriptorType Descriptor type inherited from \ref ::DescriptorBase
    //! @param binding Bingind or coordinate inside set for shaders
    //! @param args R-value arguments to construct descriptor of type \ref ::DescriptorType
    template <typename DescriptorType, typename... Args>
    void RegisterSingle(BindingType binding, Args&&...args) {
        std::vector< DescriptorBaseU > tmp;
        tmp.push_back( std::make_unique< DescriptorType >(std::forward<Args>(args)...));
        auto [_, success] = packs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(binding),
            std::forward_as_tuple(
                /* isArray  */ false,
                /* isSingle */ true, 
                std::move(tmp)
            )
        );
        log::FatalAssertNot(!success, "Binding={} occupied", binding);
    }

    template< typename DescriptorType, typename... Args>
    void RegisterArray(BindingType binding, Args&&...args) {
        AssertArrayBinding(binding);
        std::vector< DescriptorBaseU > tmp;
        tmp.push_back(std::make_unique< DescriptorType >(std::forward<Args>(args)...));
        auto [_, success] = packs.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(binding),
            std::forward_as_tuple(
                /* isArray  */ true,
                /* isSingle */ true, 
                std::move(tmp)
            )
        );
        log::FatalAssertNot(!success, "Binding={} occupied", binding);
    }

    //! Finds descriptor by frame and binding stored inside this set. If no 
    //! set are found -> terminates program.
    //! @param frame Index of frame in flight 
    //! @param binding Binding which were entered via register funcs.
    //! @return Reference to descriptor base
    DescriptorBase& Get(FrameType frame, BindingType binding);

    //! Cast version of \ref ::Get but with ready cast to type DescriptorType
    //! @tparam DescriptorType Descriptor type to cast to. Must be inherited 
    //! from \ref ::DescriptorBase
    //! @param frame Index of frame in flight 
    //! @param binding Binding which were entered via register funcs.
    //! @return Reference to casted descriptor
    template <typename DescriptorType>
    DescriptorType& Get(FrameType frame, BindingType binding) {
        return static_cast<DescriptorType&>(Get(frame, binding));
    }
    // clang-format on

    //! Returns handle to vk-descriptor-set-layout
    vk::DescriptorSetLayout& LayoutHandle() noexcept;

    //! Finds descriptor set by input index of frame in flight
    vk::DescriptorSet& Set(FrameType frame);

    //! Captures all registered bindings and returns vector of them
    std::vector<BindingType> RegisteredBindings() const;

    //                               Allocation
    // ************************************************************************

    //! Contstructo method. Continuesly calls \ref ::DescriptorBase::Initialize
    //! for each registered descriptor. Might call buffer & image creation
    //! @note must be called before adding instance of this class to \ref
    //! ::DescriptorContext
    //! @param device Nj device handle
    //! @param allocator Nj allocator handle
    void InitializeDescriptors(DeviceH device, AllocatorH allocator);

    //! Creates descriptor set layout for current instance.
    //! @note Might not be called directly
    virtual void Create(DeviceH device);

    //! Creates decriptor set vk handles for each frame individualy
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
    void AssertArrayBinding(BindingType binding) const;

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
