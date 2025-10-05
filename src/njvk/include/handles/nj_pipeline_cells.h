#pragma once
#ifndef NJ_PIPELINE_CELLS_H
#define NJ_PIPELINE_CELLS_H

#include "nj_pipeline.h"

namespace nj::ren {

// clang-format off
class PipelineBuilderCells : public PipelineBuilderBase {
public:
    virtual auto InputAssemblyState() -> vk::PipelineInputAssemblyStateCreateInfo override;
    virtual auto RasterizationState() -> vk::PipelineRasterizationStateCreateInfo override;
};
                             

}; // namespace nj::ren

#endif
