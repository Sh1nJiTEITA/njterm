#include "nj_descriptor_set.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "nj_log_scope.h"
#include "njlog.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_enums.hpp>
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
    DeviceH d,
    const vk::DescriptorSetLayoutCreateFlags& flags,
    void* pnext 
) {
    auto layout_bindings = std::vector<vk::DescriptorSetLayoutBinding>{};
    layout_bindings.reserve(packs.size());
    
    uint32_t max_binding = 0;
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
        max_binding = std::max(max_binding, binding);
    }

    auto create_info = vk::DescriptorSetLayoutCreateInfo{}
    .setBindings(layout_bindings)
    // .setPNext(pnext)
    // .setFlags(flags)
    ;

    // Descriptor array logic handling
    if (packs.contains(max_binding) && packs.at(max_binding).isArray) {
        vk::DescriptorBindingFlags binding_flags = 
            vk::DescriptorBindingFlagBits::ePartiallyBound | 
            vk::DescriptorBindingFlagBits::eVariableDescriptorCount;

        auto binding_flags_info = vk::DescriptorSetLayoutBindingFlagsCreateInfo{}
        .setBindingFlags(binding_flags);
        create_info.setPNext(binding_flags_info);
    } else { 
        layoutHandle = d->Handle().createDescriptorSetLayoutUnique(create_info);
    }
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
        
        auto it = std::max_element(
            packs.begin(), 
            packs.end(), 
            [](const auto& a, const auto& b){
                return a.first < b.first;
            }
        );
        if (it != packs.end() && it->second.isArray) { 
            const uint32_t handles_sz = it->second.handles.size();
            log::FatalAssert(
                handles_sz == 1, 
                "Pack handles size={} != 1", handles_sz
            );

            const uint32_t count = it->second.handles.back()->Count();
            log::FatalAssert(
                count > 1, 
                "Array descriptor count={} <= 1", count
            );
            
            auto count_info = vk::DescriptorSetVariableDescriptorCountAllocateInfo{}
                .setDescriptorSetCount(1)
                .setPDescriptorCounts(&count)
                ;

            info.setPNext(&count_info);
            
            auto res = device->Handle().allocateDescriptorSetsUnique(info);
            vkSets.push_back(std::move(res.back()));
        } else { 
            auto res = device->Handle().allocateDescriptorSetsUnique(info);
            vkSets.push_back(std::move(res.back()));
        }
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

DescriptorBase& DescriptorSet::Get(FrameType frame, BindingType binding) {
    log::FatalAssertNot(
        !packs.contains(binding), "Binding={} was not registered", binding
    );
    const auto& pack = packs.at(binding);
    if (pack.isSingle) {
        log::FatalAssertNot(
            pack.handles.size() != 1,
            "Invalid count of descriptors for single one", binding
        );
        return *pack.handles.back();
    } else {
        log::FatalAssertNot(
            pack.handles.size() < frame, "Frame={} > registered frames count",
            binding
        );
        return *pack.handles[frame];
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

void DescriptorSet::AssertArrayBinding(BindingType binding) const {
    for (auto& [registered_binding, _] : packs) {
        log::FatalAssertNot(
            registered_binding >= binding,
            "Cant register new descriptor as array caz some already registred "
            "binding={} >= input binding={}",
            registered_binding, binding
        );
    }
}

} // namespace nj::ren
