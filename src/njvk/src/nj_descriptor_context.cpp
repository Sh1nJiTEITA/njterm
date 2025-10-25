#include "nj_descriptor_context.h"
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "nj_log_scope.h"
#include "njlog.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <list>
#include <memory>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

namespace exp {

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
    DeviceH device, AllocatorH allocator
) {
    DEBUG_SCOPE_A("Initializing descriptors");
    for (const auto& [binding, pack] : packs) {
        DEBUG_SCOPE_A(
            "Binding={} pack.isSingle={} pack.size={}", binding, pack.isSingle,
            pack.handles.size()
        );
        for (auto& handle : pack.handles) {
            DEBUG_SCOPE_A("type={}", (int)handle->descriptorType);
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
    DEBUG_SCOPE_A("Creating descriptor set layout");
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
    DeviceH device, DescriptorPoolH pool, void* pnext
) {
    DEBUG_SCOPE_A("Allocating descriptor layouts");
    AssertDescriptorCount();

    // clang-format off
    auto layouts = std::array { layoutHandle.get() };
    for (size_t frame_idx = 0; frame_idx < framesCount; ++frame_idx) { 
        log::DebugA("-> Allocating DescriptorSet for frame {}...", frame_idx);

        // NOTE: Default vulkan behaviour: create as many descriptor sets 
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

void DescriptorSet::Write(std::vector<vk::WriteDescriptorSet>& writes) {
    std::vector<vk::DescriptorBufferInfo> buffer_infos{};
    std::vector<vk::DescriptorImageInfo> image_infos{};
    for (FrameType frame = 0; frame < framesCount; frame++) {
        for (BindingType binding : RegisteredBindings()) {
            DescriptorBase& desc = Get(frame, binding);
            writes.push_back(desc.GenWrite(buffer_infos, image_infos));
            writes.back().setDstBinding(binding);
            writes.back().setDstSet(Set(frame));
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
    DescriptorContext::BindingType binding, DescriptorContext::FrameType frame
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

void DescriptorContext::Create(DeviceH device) {
    for (auto& [layout_idx, set] : sets) {
        set->Create(device);
    }
}
void DescriptorContext::Allocate(DeviceH device, DescriptorPoolH pool) {
    for (auto& [layout_idx, set] : sets) {
        set->Allocate(device, pool);
    }
}

void DescriptorContext::Update(DeviceH device) {
    std::vector<vk::WriteDescriptorSet> writes;
    writes.reserve(150); // WARN Literal
    for (auto& [layout_idx, set] : sets) {
        set->Write(writes);
    }
    device->Handle().updateDescriptorSets(
        writes.size(), writes.data(), 0, nullptr
    );
}

void DescriptorContext::Bind(
    LayoutType layout, FrameType frame, CommandBufferH cmd,
    vk::PipelineLayout pipeline_layout
) {
    cmd->Handle().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline_layout,
        static_cast<uint32_t>(layout), std::vector{sets[layout]->Set(frame)}, {}
    );
}

std::vector<vk::DescriptorSetLayout> DescriptorContext::AllLayouts() {
    std::vector<vk::DescriptorSetLayout> l;
    l.reserve(100); // WARN Literal
    for (auto& [layout, set] : sets) {
        l.push_back(set->LayoutHandle());
    }
    std::reverse(l.begin(), l.end());
    return l;
}

}; // namespace exp

// clang-format off

class DescriptorSet {
public:
    
    //! Storing each descriptor set for each frame separatly.
    //! Each pack can have multiple descriptors -> storing
    //! them inside for each binding in vector
    struct FrameDescriptorSetPack {
        vk::UniqueDescriptorSet set;
        //! Index => binding
        std::unordered_map<size_t, DescriptorU> descriptors;
    };

    static size_t descriptorSetCountPerClass;

public:
    //! The only descriptor set ctor
    //! @param packs Map with keys as bindings ? and ... ? 
    DescriptorSet(const vk::DescriptorSetLayoutCreateInfo& create_info,
                  ren::DeviceH device, 
                  std::vector<FrameDescriptorSetPack>&& packs, 
                  std::vector<DescriptorU>&& single_desc) 
        : framePacks{ std::move(packs) }
        , singleDescriptors{ std::move(single_desc) } 
    {
        log::DebugA("DescriptorSet::ctor packs.size={}", framePacks.size());
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        std::set<uint32_t> unique;
        log::DebugA("Searching for unique bindings inside perframe descriptor list");

        const auto add = [&unique, &bindings](DescriptorU& desc){
            auto info = desc->LayoutBinding();
            auto [it, success] = unique.insert(info.binding);
            if (success) {
                log::DebugA("Unique binding={}", info.binding);
                bindings.push_back(std::move(info));
            }
        };

        for (size_t frame_index = 0; frame_index < framePacks.size(); ++frame_index) {
            for (auto &[bindind, desc] : framePacks[frame_index].descriptors) 
            { 
                add(desc); 
            }
        }

        log::DebugA("Searching for unique bindings inside single descriptor list");
        for (auto& single_desc : singleDescriptors) { 
            add(single_desc); 
        }

        auto layout_create_info = create_info;
        layout_create_info.setBindings(bindings);

        layout = device->Handle().createDescriptorSetLayoutUnique(layout_create_info);

        // auto layout_create_info = vk::DescriptorSetLayoutCreateInfo{}
        //     .setBindings(bindings)
        //     ;

         // --------------- BEGIN: VK_EXT_descriptor_indexing -----------------
        // Create binding flags for all bindings
        // std::vector<vk::DescriptorBindingFlagsEXT> bindingFlags(bindings.size(), 
        //
        //     // allow some array elements to be unbound
        //     vk::DescriptorBindingFlagBitsEXT::ePartiallyBound  |       
        //
        //     // allow updating descriptors after pipeline bind
        //     vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind |      
        //
        //     // allows bindless / variable-length arrays
        //     vk::DescriptorBindingFlagBitsEXT::eVariableDescriptorCount
        // );
        //
        // auto bindingFlagsInfo = vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT{}
        //     .setBindingFlags(bindingFlags)
        //     ;

        // Attach pNext to the layout create info
        // auto layout_create_info = vk::DescriptorSetLayoutCreateInfo{}
        //     .setBindings(bindings)
        //     .setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
        //     .setPNext(&bindingFlagsInfo) // <<--- critical for bindless support
        //     ;
        // --------------- END: VK_EXT_descriptor_indexing --------------------
        
    }

    auto Layout() noexcept -> vk::DescriptorSetLayout& { return *layout; }

    void Allocate(ren::DeviceH device, ren::DescriptorPoolH pool, void* pnext) { 
        log::DebugA("Allocating descriptor set...");
        const size_t frames_count = framePacks.size();
        const auto layouts = std::array{ layout.get() };
        for (size_t frame_idx = 0; frame_idx < frames_count; ++frame_idx) { 
            log::DebugA("Allocating DescriptorSet for frame {}...", frame_idx);
            auto info = vk::DescriptorSetAllocateInfo{}
                .setDescriptorPool(pool->CHandle())
                .setDescriptorSetCount(descriptorSetCountPerClass)
                .setSetLayouts(layouts)
                ;
            if (pnext) {
                info.setPNext(pnext);
            }
            auto res = device->Handle().allocateDescriptorSetsUnique(info);
            assert(res.size() == descriptorSetCountPerClass);
            framePacks[frame_idx].set = std::move(res.front());
        }
    }

    auto Write(size_t frame, std::list<vk::DescriptorBufferInfo>& buffer_infos,
                              std::list<vk::DescriptorImageInfo>& image_infos) -> std::vector<vk::WriteDescriptorSet> { 
        log::Debug("------> Make writes for frame={}", frame);
        std::vector<vk::WriteDescriptorSet> writes;
        const auto& frame_pack = framePacks[frame];
        for (auto& [binding, desc] : frame_pack.descriptors) { 
            log::Debug("--------> Adding write for descriptor with index (binding)={} and hasBuffer={}, hasImage={}", binding, desc->HasBuffer(), desc->HasImage());
            const auto& buffer_info_it = *buffer_infos.insert(buffer_infos.end(), desc->BufferInfo());
            const auto& image_info_it = *image_infos.insert(image_infos.end(), desc->ImageInfo());
            auto write_info = vk::WriteDescriptorSet{}
                .setDstSet(frame_pack.set.get())
                .setDstBinding(desc->Binding())
                .setDescriptorCount(descriptorSetCountPerClass)
                .setDescriptorType(desc->DescriptorType())
                .setPImageInfo(desc->HasImage() ? &image_info_it : nullptr)
                .setPBufferInfo(desc->HasBuffer() ? &buffer_info_it : nullptr)
                ;
            writes.push_back(std::move(write_info));
        }
        
        for (auto& desc : singleDescriptors) { 
            const auto& buffer_info_it = *buffer_infos.insert(buffer_infos.end(), desc->BufferInfo());
            const auto& image_info_it = *image_infos.insert(image_infos.end(), desc->ImageInfo());
            auto write_info = vk::WriteDescriptorSet{}
                .setDstSet(frame_pack.set.get())
                .setDstBinding(desc->Binding())
                .setDescriptorCount(1)
                .setDescriptorType(desc->DescriptorType())
                .setPImageInfo(desc->HasImage() ? &image_info_it : nullptr)
                .setPBufferInfo(desc->HasBuffer() ? &buffer_info_it : nullptr)
                ;
            writes.push_back(std::move(write_info));
        }
        return writes;
    }

    
    
    auto Descriptor(size_t frame, size_t binding) -> Descriptor& { 
        assert(framePacks.size() > frame && "Frame index is too big");
        assert(framePacks[frame].descriptors.contains(binding) && "Binding does not exist");
        return *framePacks[frame].descriptors[binding];
    }

    auto DescriptorSetHandle(size_t frame) -> vk::DescriptorSet& { 
        assert(framePacks.size() > frame && "Frame index is too big");
        return *framePacks[frame].set;
    }

private:
    // vk::SharedDescriptorSetLayout layout;
    vk::UniqueDescriptorSetLayout layout;
    //! Holding frame packs continuously in memory
    //! Index => Frame number 
    std::vector< FrameDescriptorSetPack > framePacks;
    std::vector< DescriptorU > singleDescriptors;
};

size_t DescriptorSet::descriptorSetCountPerClass = 1;


class DescriptorContext::Impl {
private:
    struct DescriptorTempCreateInfo {
        size_t layout;
        size_t binding;
        std::vector<DescriptorU> descriptorPerFrame;
    };

public:
    Impl(ren::DeviceH device, DescriptorPoolH pool, AllocatorH allocator, size_t frames) 
        : pool { pool } 
        , device { device }
        , allocator { allocator } 
        , frames { frames }
    {
        // Your internal data and setup
    }
    virtual ~Impl() {} 

    void RegisterLayout(
        size_t layout, 
        const vk::DescriptorSetLayoutCreateInfo& info,
        const vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT& var_info
    ) {
        if (setsCreateInfo.contains(layout) || setsVarAllocateInfo.contains(layout)) {
            log::FatalExit("Cant register layout={}. Already registerd", layout);
        }

        setsCreateInfo[layout] = info;
        setsVarAllocateInfo[layout] = var_info;
    }

    void Add(size_t layout, size_t binding, std::vector<DescriptorU>&& descriptors) {
        log::Debug(
            "-> Inner add new descriptor with layout={} binding={} frames={}", 
            layout, binding, frames
        );
        for (auto& desc : descriptors) { 
            desc->CreateBuffers(device, allocator);
            desc->CreateImages(device, allocator);
            desc->CreateViews(device, allocator);
        }
        tempCreateInfos.emplace(layout, binding, std::move(descriptors)); 
    }
    
    void CreateLayouts() { 
        using layout_idx = size_t;
        using binding_idx = size_t;
        
        log::Info("Creating descriptor context layouts... STARTED");
        
        std::unordered_map<layout_idx, 
                           std::unordered_map<binding_idx, 
                                              DescriptorTempCreateInfo>> m;

        log::Debug("Processing added descriptors... STARTED");
        while (!tempCreateInfos.empty()) { 
            auto desc = std::move(tempCreateInfos.top());
            tempCreateInfos.pop();

            const size_t layout = desc.layout;
            const size_t binding = desc.binding;

            log::Debug("Processing descriptor with [layout={} binding={}]", layout, binding);

            auto& set = m[layout];
            if (set.empty()) { 
                log::Debug("New desc with layout={} found in descriptors data", layout);
            }
            auto [_, new_desc_in_set_status] = m.at(layout).insert({binding, std::move(desc)});
            if (!new_desc_in_set_status) { 
                log::FatalExit("Cant add descriptor with binding={} to set={}, already exists", layout, binding);
            }
            else { 
                log::Debug("New descriptor with binding={} added to set with index={}", binding, layout);
            }
        }
        log::Debug("Processing added descriptors... DONE");

        for (auto& [set_i, bindings_map] : m) {
            log::Debug("--> Creating descriptor-set-layout with index={} and descriptors count={}",
                       set_i, bindings_map.size());

            std::vector<DescriptorU> single_desc;
            std::vector<DescriptorSet::FrameDescriptorSetPack> current_packs;
            current_packs.resize(frames);

            for (auto &[binding, tmpStorage] : bindings_map) {
                log::Debug("----> Going through bindings map with binding={} as key", binding);
                const size_t packs_count = tmpStorage.descriptorPerFrame.size();

                // assert(packs_count == frames &&
                //        "Descriptor count must be the same as frames on flight "
                //        "count");
                if (packs_count == frames) { 
                    log::Debug("----> Descriptor resources are allocated "
                               "for each frame SEPARETLY", binding);
                    for (size_t pack_index = 0; pack_index < packs_count; ++pack_index) {
                        log::Debug("------> Going through packs with pack={} as frame", pack_index);
                        // FIXME: ... 
                        //============================================
                        auto& pack = current_packs[pack_index];
                        auto [it, success]  = pack.descriptors.insert(std::make_pair(binding, std::move(tmpStorage.descriptorPerFrame[pack_index])));
                        if (!success) { 
                            log::FatalExit("Cant assign tmp storage to binding key");
                        }
                    }
                } else { 
                    log::Debug("----> Descriptor resources are allocated "
                               "for each frame SHAREDLY", binding);
                    single_desc.push_back(std::move(tmpStorage.descriptorPerFrame[0]));
                }
            }
            for (size_t frame = 0; frame < frames; ++frame) { 
                log::Debug("==> In frame pack with index={} as frame descriptor count={}", frame, current_packs[frame].descriptors.size());
            }

            auto complete_set = std::make_unique<DescriptorSet>(
                // If some layout were rigistered with specific settings:
                // stored create info will be used for desc set layout creation
                setsCreateInfo.contains(set_i) ? setsCreateInfo[set_i] 
                                               : vk::DescriptorSetLayoutCreateInfo{},
                device, 
                std::move(current_packs), 
                std::move(single_desc)
            );
            sets.insert_or_assign(set_i, std::move(complete_set));
        }
        log::Debug("Creating descriptor context layouts... DONE");
    };

    // clang-format on
    void AllocateSets() {
        log::Debug(
            "Allocating descriptor sets for descriptor context... STARTED"
        );
        for (auto& [layout_index, set] : sets) {
            log::Debug(
                "Allocating sets for layout={}", static_cast<int>(layout_index)
            );
            void* var_info = nullptr;
            if (setsVarAllocateInfo.contains(layout_index)) {
                var_info = &setsVarAllocateInfo[layout_index];
            }
            set->Allocate(device, pool, var_info);
        }
        log::Debug("Allocating descriptor sets for descriptor context... DONE");
    }

    void UpdateSets() {
        log::Debug("Updating descriptor sets... STARTED");
        // FIXME: remove literal with logic
        const size_t TRUE_WRITES_COUNT = 150;
        for (int frame = 0; frame < frames; ++frame) {
            log::Debug("--> Updating sets for frame={}", frame);
            std::vector<vk::WriteDescriptorSet> writes;
            writes.reserve(TRUE_WRITES_COUNT);
            std::list<vk::DescriptorBufferInfo> binfos;
            std::list<vk::DescriptorImageInfo> iinfos;
            // FIXME: Missing texel ...
            for (auto& [layout, pset] : sets) {
                log::Debug("----> Updating set with layout={}", layout);
                auto new_writes = pset->Write(frame, binfos, iinfos);
                std::copy(
                    new_writes.begin(), new_writes.end(),
                    std::back_inserter(writes)
                );
            }
            log::Debug("==> Calling vk-update func for frame={}", frame);
            device->Handle().updateDescriptorSets(
                writes.size(), writes.data(), 0, nullptr
            );
        }
        log::Debug("Updating descriptor sets... DONE");
    }

    void BindSets(
        size_t layout, size_t frame, CommandBufferH cb,
        vk::PipelineLayout pllayout
    ) {
        cb->Handle().bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, pllayout,
            static_cast<uint32_t>(layout),
            std::vector{sets[layout]->DescriptorSetHandle(frame)}, {}
        );
    }

    auto AllLayouts() -> std::vector<vk::DescriptorSetLayout> {
        std::vector<vk::DescriptorSetLayout> l;
        l.reserve(10);
        for (auto& [layout, set] : sets) {
            l.push_back(set->Layout());
        }
        std::reverse(l.begin(), l.end());
        return l;
    }

    auto Get(size_t frame, size_t layout, size_t binding) -> Descriptor& {
        return sets[layout]->Descriptor(frame, binding);
    };

    // clang-format off


private:
    DescriptorPoolH pool;
    AllocatorH allocator;
    DeviceH device;
    const size_t frames;

    std::stack<DescriptorTempCreateInfo> tempCreateInfos;
    std::unordered_map<size_t, std::unique_ptr< DescriptorSet >> sets;
    std::unordered_map<size_t, vk::DescriptorSetLayoutCreateInfo> setsCreateInfo;
    std::unordered_map<size_t, vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT> setsVarAllocateInfo;
};


DescriptorContext::DescriptorContext(DeviceH device, DescriptorPoolH pool, AllocatorH allocator, size_t frames) 
    : impl(std::make_unique<Impl>(device, pool, allocator, frames)) {}


void DescriptorContext::RegisterLayout(size_t layout, const vk::DescriptorSetLayoutCreateInfo& info,
                                       const vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT& var_info) {
    impl->RegisterLayout(layout, info, var_info);
}

void DescriptorContext::Add(size_t layout, 
                            size_t binding, 
                            std::vector<DescriptorU>&& descriptor) { 
    impl->Add(layout, binding, std::move(descriptor));
}

DescriptorContext::~DescriptorContext() { 

}



void DescriptorContext::CreateLayouts() { impl->CreateLayouts(); }
void DescriptorContext::AllocateSets() { impl->AllocateSets(); } 
void DescriptorContext::UpdateSets() { impl->UpdateSets(); } 
void DescriptorContext::BindSets(size_t layout, size_t frame, CommandBufferH cb, vk::PipelineLayout pllayout) { impl->BindSets(layout, frame, cb,  pllayout); } 
auto DescriptorContext::AllLayouts() -> std::vector<vk::DescriptorSetLayout> { return impl->AllLayouts(); }
auto DescriptorContext::Get(size_t frame, size_t layout, size_t binding) -> Descriptor&{ return impl->Get(frame, layout, binding); }



} // namespace nj::ren
