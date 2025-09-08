#pragma once
#ifndef NJ_PIPELINE_GUIDELINES_H
#define NJ_PIPELINE_GUIDELINES_H

#include "nj_pipeline.h"

namespace nj::ren {

// clang-format off
class PipelineBuilderGuidelines : public PipelineBuilderBase {
public:
    virtual auto InputAssemblyState() -> vk::PipelineInputAssemblyStateCreateInfo override;
    virtual auto RasterizationState() -> vk::PipelineRasterizationStateCreateInfo override;
private:
    ren::VarHandles h;
};
                             

}; // namespace nj::ren

#endif
