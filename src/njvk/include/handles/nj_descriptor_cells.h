#ifndef NJ_DESCRIPTOR_CELLS
#define NJ_DESCRIPTOR_CELLS

#include "nj_descriptor.h"
#include "nj_text_buffer.h"

namespace nj::ren {

struct DescriptorCells : public Descriptor {
    DescriptorCells(size_t layout, size_t binding, buf::TextBufferH buf);

    virtual void CreateBuffer(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateImage(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateView(ren::DeviceH device, ren::AllocatorH allocator);

    void Update();

private:
    buf::TextBufferH textBuffer;
};

}; // namespace nj::ren

#endif
