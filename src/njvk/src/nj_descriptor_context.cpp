#include "nj_descriptor_context.h"
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
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

void DescriptorContext::Create(DeviceH device) {
    DEBUG_SCOPE_A("Creating descriptor sets layouts");
    for (auto& [layout, set] : sets) {
        DEBUG_SCOPE_A("Creating descriptor set with layout={}", layout);
        set->Create(device);
    }
}
void DescriptorContext::Allocate(DeviceH device, DescriptorPoolH pool) {
    DEBUG_SCOPE_A("Allocating descriptor sets");
    for (auto& [layout, set] : sets) {
        DEBUG_SCOPE_A("Allocating descriptor sets for layout={}", layout);
        set->Allocate(device, pool);
    }
}

// clang-format off
void DescriptorContext::Update(DeviceH device) {
    DEBUG_SCOPE_A("Updating descriptor sets");

    for (auto& [layout, set] : sets) {
        DEBUG_SCOPE_A("Make writes for descriptor set with layout={}", layout);
            
        std::vector<vk::WriteDescriptorSet> writes{}; writes.reserve(256);
        std::vector<vk::DescriptorBufferInfo> buffer_infos{}; buffer_infos.reserve(256);
        std::vector<vk::DescriptorImageInfo> image_infos{}; image_infos.reserve(256);

        set->Write(writes, buffer_infos, image_infos);

        log::DebugA("Calling vkUpdateDescriptorSets");
        device->Handle().updateDescriptorSets(
            writes.size(), writes.data(), 0, nullptr
        );
    }
    
}
// clang-format on

std::vector<vk::DescriptorSetLayout> DescriptorContext::AllLayouts() {
    std::vector<vk::DescriptorSetLayout> l;
    l.reserve(100); // WARN Literal
    for (auto& [layout, set] : sets) {
        l.push_back(set->LayoutHandle());
    }
    std::reverse(l.begin(), l.end());
    return l;
}

void DescriptorContext::Bind(
    LayoutType layout,
    FrameType frame,
    CommandBufferH cmd,
    vk::PipelineLayout pipeline_layout
) {
    cmd->Handle().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline_layout,
        static_cast<uint32_t>(layout), std::vector{sets[layout]->Set(frame)}, {}
    );
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
