#include "nj_descriptor.h"
#include "njvkutils.h"
#include <vulkan/vulkan_shared.hpp>

namespace nj::ren {

// clang-format off
DescriptorPool::DescriptorPool(ren::DeviceH device) {
    ren::VarHandles h{};

    auto info = vk::DescriptorPoolCreateInfo{}
        .setPoolSizes(
            h.Handle(std::vector{ vk::DescriptorPoolSize{}.setDescriptorCount(10) })
        )
        .setMaxSets(10)
        ;

    handle = vk::SharedDescriptorPool(device->Handle()->createDescriptorPool(info), device->Handle());
}
// clang-format on

auto DescriptorPool::HandleName() const noexcept -> std::string {
    return "DescriptorPool";
}
} // namespace nj::ren
