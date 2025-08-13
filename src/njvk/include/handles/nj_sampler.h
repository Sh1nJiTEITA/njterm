#pragma once
#ifndef NJ_SAMPLER_H
#define NJ_SAMPLER_H

#include "nj_device.h"
#include "nj_handle.h"

namespace nj::ren {

class Sampler : public VulkanObject<vk::Sampler> {
  public:
    Sampler(DeviceH device);

    virtual std::string HandleName() const noexcept override;
};
using SamplerH = std::shared_ptr<Sampler>;

} // namespace nj::ren

#endif
