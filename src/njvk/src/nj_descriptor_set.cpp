#include "nj_descriptor_set.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "nj_log_scope.h"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

DescriptorSet::DescriptorSet(DescriptorSet::FrameType frames_count)
    : framesCount{frames_count} {}

vk::DescriptorSetLayout& DescriptorSet::LayoutHandle() noexcept {
    return layoutHandle.get();
}

vk::DescriptorSet& DescriptorSet::Set(DescriptorSet::FrameType frame) {
    if (frame > vkSets.size()) {
        log::FatalExitInternal(
            "Cant get descriptor set. Input frame index={} > vkSets.size()={}",
            frame, vkSets.size()
        );
    }
    return vkSets[frame].get();
}

std::vector<DescriptorSet::BindingType>
DescriptorSet::RegisteredBindings() const {
    std::vector<BindingType> bindings;
    bindings.reserve(packs.size());
    for (const auto& pack : packs) {
        bindings.push_back(pack.first);
    }
    return bindings;
}

void DescriptorSet::InitializeDescriptors(
    DeviceH device,
    AllocatorH allocator
) {
    DEBUG_SCOPE_A("Initializing descriptors");
    for (const auto& [binding, pack] : packs) {
        DEBUG_SCOPE_A(
            "Initializing descriptor with binding={} pack.isSingle={} "
            "pack.size={}",
            binding, pack.isSingle, pack.handles.size()
        );
        size_t i = 0;
        for (auto& handle : pack.handles) {
            DEBUG_SCOPE_A(
                "Initializing handle #{} with type={}", i++,
                static_cast<int>(handle->descriptorType)
            );
            handle->Initialize(device, allocator);
        }
    }
}

// clang-format off
void DescriptorSet::InternalCreate(
    DeviceH device,
    const vk::DescriptorSetLayoutCreateFlags& flags,
    void* pnext 
) {
    auto layout_bindings = std::vector<vk::DescriptorSetLayoutBinding>{};
    layout_bindings.reserve(packs.size());
    for (const auto& [binding, pack] : packs) {
        log::DebugA(
            "Adding binding={} with pack: isSingle={}, size={}", 
            binding, pack.isSingle, pack.handles.size()
        );
        auto layout_binding = vk::DescriptorSetLayoutBinding{}
            .setBinding(binding)
            .setDescriptorType(pack.handles.back()->descriptorType)
            .setStageFlags(pack.handles.back()->shaderStages)
            // .setDescriptorCount(pack.handles.size())
            .setDescriptorCount(1)
            ;
        layout_bindings.push_back(layout_binding);
    }

    auto create_info = vk::DescriptorSetLayoutCreateInfo{}
    .setBindings(layout_bindings)
    // .setPNext(pnext)
    // .setFlags(flags)
    ;

    layoutHandle = device->Handle().createDescriptorSetLayoutUnique(create_info);
}
// clang-format on

void DescriptorSet::Create(DeviceH device) {
    InternalCreate(device, {}, nullptr);
}

void DescriptorSet::InternalAllocate(
    DeviceH device,
    DescriptorPoolH pool,
    void* pnext
) {
    AssertDescriptorCount();

    // clang-format off
    auto layouts = std::array { layoutHandle.get() };
    for (size_t frame_idx = 0; frame_idx < framesCount; ++frame_idx) { 
        log::DebugA("Allocating DescriptorSet for frame {}...", frame_idx);

        //  NOTE: Default vulkan behaviour: create as many descriptor sets 
        // as layouts. For current codebase we have single layout per 
        // ren::DescriptorSet but inside we have mulitple vulkan desciptor set
        // handles for each frame. 
        // 
        // No reason to allocate multiple descriptor sets per frame via using
        // .setDescriptorSetCount( ... ) because of above mentioned reason
        
        auto info = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(pool->CHandle())
        .setDescriptorSetCount(1)
        .setSetLayouts(layouts)
        ;
        if (pnext) {
            info.setPNext(pnext);
        }
        auto res = device->Handle().allocateDescriptorSetsUnique(info);
        vkSets.push_back(std::move(res.back()));
    }
    // clang-format on
}

void DescriptorSet::Allocate(DeviceH device, DescriptorPoolH pool) {
    InternalAllocate(device, pool, nullptr);
}

void DescriptorSet::Write(
    std::vector<vk::WriteDescriptorSet>& writes,
    std::vector<vk::DescriptorBufferInfo>& buffer_infos,
    std::vector<vk::DescriptorImageInfo>& image_infos
) {
    for (FrameType frame = 0; frame < framesCount; frame++) {
        for (BindingType binding : RegisteredBindings()) {
            const DescriptorBase& desc = Get(frame, binding);
            const Pack& pack = packs.at(binding);

            auto& last = writes.emplace_back();
            last.setDstBinding(binding);
            last.setDstSet(Set(frame));
            // NOTE: Moved to DescriptorBase implementations
            // last.setDescriptorCount(1);
            last.setDescriptorType(desc.descriptorType);
            desc.FillWriteWithResourcesInfo(last, buffer_infos, image_infos);
        }
    }
}

void DescriptorSet::AssertDescriptorCount() const {
    for (const auto& [binding, pack] : packs) {
        if (!pack.isSingle && pack.handles.size() != framesCount) {
            log::FatalExitInternal(
                "Descriptor count={} and frame count={} mismatch during "
                "descriptor "
                "set allocation",
                pack.handles.size(), framesCount
            );
        }
    }
}

void DescriptorContext::Add(LayoutType layout, DescriptorSetU&& set) {
    auto [added_set, success] = sets.emplace(layout, std::move(set));
    if (!success) {
        log::FatalExitInternal("Cant add set with layout={}", layout);
    }
}

DescriptorBase& DescriptorContext::GetDescriptor(
    DescriptorContext::LayoutType layout,
    DescriptorContext::BindingType binding,
    DescriptorContext::FrameType frame
) {
    return GetSet(layout).Get(frame, binding);
}

DescriptorSet& DescriptorContext::GetSet(DescriptorContext::LayoutType layout) {
    if (!sets.contains(layout)) {
        log::FatalExit(
            "Wrong layout={} for getting descriptor from descriptor context",
            layout
        );
    }
    return *sets[layout];
}
} // namespace nj::ren
