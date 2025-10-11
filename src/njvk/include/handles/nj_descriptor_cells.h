#ifndef NJ_DESCRIPTOR_CELLS
#define NJ_DESCRIPTOR_CELLS

#include "nj_descriptor.h"
#include "nj_text_buffer.h"

namespace nj {
#ifndef NJ_CHAR_STRUCT
#define NJ_CHAR_STRUCT

struct alignas(16) SingleCharTextureData {
    glm::vec4 uv; // 4 * 4 = 16 Bytes
};

#endif

namespace ren {

struct DescriptorCells : public Descriptor {
    DescriptorCells(size_t layout, size_t binding, buf::TextBufferH buf);

    virtual void CreateBuffers(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateImages(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateViews(ren::DeviceH device, ren::AllocatorH allocator);

    void Update();

private:
    buf::TextBufferH textBuffer;
};
//
//
//
//

//
//
struct DescriptorCharactersMeta : public Descriptor {
    DescriptorCharactersMeta(size_t layout, size_t binding, BufferU&& buf);

    virtual void CreateBuffers(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateImages(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateViews(ren::DeviceH device, ren::AllocatorH allocator);

    void Update();

private:
    size_t charsCount;
};

BufferU CreateCharactersMetaBuffer(DeviceH d, AllocatorH a, size_t cells_count);
BufferU CreateCharactersMetaBuffer(
    DeviceH d, AllocatorH a, const std::vector<SingleCharTextureData>&
);

} // namespace ren
} // namespace nj
#endif
