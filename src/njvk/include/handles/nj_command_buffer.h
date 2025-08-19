#pragma once
#ifndef NJ_COMMAND_BUFFER_H
#define NJ_COMMAND_BUFFER_H

#include "nj_command_pool.h"
#include "nj_device.h"
#include "nj_handle.h"
#include <memory>

namespace nj::ren {

// clang-format off
class CommandBuffer : public VulkanObjectNative<vk::CommandBuffer> {
  public:
    CommandBuffer(ren::DeviceH device, ren::CommandPoolH command_pool);
    auto HandleName() const noexcept -> std::string;
};
using CommandBufferH = std::shared_ptr<CommandBuffer>;

// clang-format on
//
} // namespace nj::ren

#endif
