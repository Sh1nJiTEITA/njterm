#include "nj_descriptor_context.h"
#include "nj_command_buffer.h"
#include "nj_descriptor.h"
#include "nj_device.h"
#include "nj_handle.h"
#include "njlog.h"
#include <cassert>
#include <cstdint>
#include <list>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off

class DescriptorSet {
public:
    
    //! Storing each descriptor set for each frame separatly.
    //! Each pack can have multiple descriptors -> storing
    //! them inside for each binding in vector
    struct FrameDescriptorSetPack {
        vk::SharedDescriptorSet set;
        //! Index => binding
        std::vector<DescriptorU> descriptors;
    };

    static size_t descriptorSetCountPerClass;

public:
    //! The only descriptor set ctor
    //! @param packs Map with keys as bindings ? and ... ? 
    DescriptorSet(ren::DeviceH device, 
                  std::vector<FrameDescriptorSetPack>&& packs, 
                  std::vector<DescriptorU>&& single_desc) 
        : framePacks{ std::move(packs) }
        , singleDescriptors{ std::move(single_desc) } 
    {
        log::Debug("DescriptorSet::ctor packs.size={}", framePacks.size());
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        std::set<uint32_t> unique;
        log::Debug("Searching for unique bindings inside perframe descriptor list");

        const auto add = [&unique, &bindings](DescriptorU& desc){
            auto info = desc->LayoutBinding();
            auto [it, success] = unique.insert(info.binding);
            if (success) {
                log::Debug("--> Unique binding={}", info.binding);
                bindings.push_back(std::move(info));
            }
        };

        for (size_t frame_index = 0; frame_index < framePacks.size(); ++frame_index) {
            for (auto &desc : framePacks[frame_index].descriptors) { add(desc); }
        }
        log::Debug("Searching for unique bindings inside single descriptor list");
        for (auto& single_desc : singleDescriptors) { add(single_desc); }

        auto layout_create_info = vk::DescriptorSetLayoutCreateInfo{}
            .setBindings(bindings)
            ;

        vk::DescriptorSetLayout res = device->Handle()->createDescriptorSetLayout(layout_create_info);
        layout = vk::SharedDescriptorSetLayout(res, device->Handle());
    }

    auto Layout() noexcept -> vk::SharedDescriptorSetLayout { return layout; }

    void Allocate(ren::DeviceH device, ren::DescriptorPoolH pool) { 
        log::Debug("Allocating descriptor set...");
        const size_t frames_count = framePacks.size();
        const auto layouts = std::array{ layout.get() };
        // log::Debug("Allocating for perframe descriptors...");
        for (size_t frame_idx = 0; frame_idx < frames_count; ++frame_idx) { 
            log::Debug("--> Allocating DescriptorSet for frame {}...", frame_idx);
            auto info = vk::DescriptorSetAllocateInfo{}
                .setDescriptorPool(pool->CHandle())
                .setDescriptorSetCount(descriptorSetCountPerClass)
                .setSetLayouts(layouts)
                ;
            auto res = device->Handle()->allocateDescriptorSets(info);
            assert(res.size() == descriptorSetCountPerClass);
            framePacks[frame_idx].set = vk::SharedDescriptorSet(
                res.front(), device->Handle(), pool->Handle()
            );
        }
    }

    auto Write(size_t frame, std::list<vk::DescriptorBufferInfo>& buffer_infos,
                              std::list<vk::DescriptorImageInfo>& image_infos) -> std::vector<vk::WriteDescriptorSet> { 
        log::Debug("------> Make writes for frame={}", frame);
        std::vector<vk::WriteDescriptorSet> writes;
        const auto& frame_pack = framePacks[frame];
        for (size_t desc_idx = 0; desc_idx < frame_pack.descriptors.size(); ++desc_idx) { 
            const auto& desc = frame_pack.descriptors[desc_idx];
            log::Debug("--------> Adding write for descriptor with index (binding)={} and hasBuffer={}, hasImage={}", desc_idx, desc->HasBuffer(), desc->HasImage());
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
        assert(framePacks[frame].descriptors.size() > binding && "Binding index is too big");
        return *framePacks[frame].descriptors[binding];
    }

    auto DescriptorSetHandle(size_t frame) -> vk::SharedDescriptorSet { 
        assert(framePacks.size() > frame && "Frame index is too big");
        return framePacks[frame].set;
    }

private:
    vk::SharedDescriptorSetLayout layout;
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

    void Add(size_t layout, size_t binding, std::vector<DescriptorU>&& descriptors) {
        log::Debug(
            "Adding new descriptor with layout={} & binding={} for num frames={}", 
            layout, binding, frames
        );
        for (auto& desc : descriptors) { 
            desc->CreateBuffer(device, allocator);
            desc->CreateImage(device, allocator);
            desc->CreateView(device, allocator);
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

            log::Debug("Processing descriptor with layout={}, binding={}", layout, binding);

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
                        current_packs[pack_index].descriptors.push_back(
                            std::move(tmpStorage.descriptorPerFrame[pack_index])
                        );
                    }
                } else { 
                    log::Debug("----> Descriptor resources are allocated "
                               "for each frame SHAREDLY", binding);
                    single_desc.push_back(std::move(tmpStorage.descriptorPerFrame[0]));
                }
                // log::Debug("current_packs[pack_index].descriptors={}", current_packs[pack_index].descriptors.size());
            }
            for (size_t frame = 0; frame < frames; ++frame) { 
                log::Debug("==> In frame pack with index={} as frame descriptor count={}", frame, current_packs[frame].descriptors.size());
            }

            auto complete_set = std::make_unique<DescriptorSet>(
                device, 
                std::move(current_packs), 
                std::move(single_desc)
            );
            sets.insert_or_assign(set_i, std::move(complete_set));
            // sets.emplace(set_i, std::move(complete_set));
        }
        log::Debug("Creating descriptor context layouts... DONE");
    };
    // clang-format on
    void AllocateSets() {
        log::Debug(
            "Allocating descriptor sets for descriptor context... STARTED");
        for (auto &[layout_index, set] : sets) {
            log::Debug("Allocating sets for layout={}",
                       static_cast<int>(layout_index));
            set->Allocate(device, pool);
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
            for (auto &[layout, pset] : sets) {
                log::Debug("----> Updating set with layout={}", layout);
                auto new_writes = pset->Write(frame, binfos, iinfos);
                std::copy(new_writes.begin(), new_writes.end(),
                          std::back_inserter(writes));
            }
            log::Debug("==> Calling vk-update func for frame={}", frame);
            device->Handle()->updateDescriptorSets(writes.size(), writes.data(),
                                                   0, nullptr);
        }
        log::Debug("Updating descriptor sets... DONE");
    }

    void BindSets(size_t layout, size_t frame, CommandBufferH cb,
                  vk::SharedPipelineLayout pllayout) {
        cb->Handle()->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, *pllayout,
            static_cast<uint32_t>(layout),
            std::vector{*sets[layout]->DescriptorSetHandle(frame)}, {});
    }

    auto AllLayouts() -> std::vector<vk::SharedDescriptorSetLayout> {
        std::vector<vk::SharedDescriptorSetLayout> l;
        for (auto &[layout, set] : sets) {
            l.push_back(set->Layout());
        }
        // std::reverse(l.begin(), l.end());
        return l;
    }

    auto Get(size_t frame, size_t layout, size_t binding) -> Descriptor & {
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
};


DescriptorContext::DescriptorContext(DeviceH device, DescriptorPoolH pool, AllocatorH allocator, size_t frames) 
    : impl(std::make_unique<Impl>(device, pool, allocator, frames)) {}

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
void DescriptorContext::BindSets(size_t layout, size_t frame, CommandBufferH cb, vk::SharedPipelineLayout pllayout) { impl->BindSets(layout, frame, cb,  pllayout); } 
auto DescriptorContext::AllLayouts() -> std::vector<vk::SharedDescriptorSetLayout> { return impl->AllLayouts(); }
auto DescriptorContext::Get(size_t frame, size_t layout, size_t binding) -> Descriptor&{ return impl->Get(frame, layout, binding); }



} // namespace nj::ren
