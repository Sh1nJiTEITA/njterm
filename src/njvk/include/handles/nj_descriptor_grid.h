#pragma once
#ifndef NJ_DESCRIPTOR_GRID_H
#define NJ_DESCRIPTOR_GRID_H
#include "nj_descriptor.h"

namespace nj::ren {

// clang-format off



struct DescriptorGrid : public Descriptor {
    DescriptorGrid(size_t layout, size_t binding);

    //! Must be aligned for std140 -> each vecN field is
    //! aligned to 16 bytes
    struct alignas(32) Data { 
        glm::ivec2 extent;      //!< size is 4 * 2 = 8
        glm::ivec2 faceSize;    //!< size is 4 * 2 = 8
        glm::ivec2 pageSize;    //!< size is 4 * 2 = 8 
    };

    virtual void CreateBuffer(DeviceH device, AllocatorH allocator) override;
    virtual void CreateImage(DeviceH device, AllocatorH allocator) override;
    virtual void CreateView(DeviceH device, AllocatorH allocator) override;
    

    //! Updates descriptor buffer with new values
    //! @param ext Current render extent
    //! @param face_size Current enabled face (font) size
    void Update(const glm::ivec2& ext, const glm::ivec2 face_size, 
                const glm::ivec2& page_size);
};

// clang-format on

} // namespace nj::ren

#endif
