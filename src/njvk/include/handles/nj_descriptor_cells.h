#ifndef NJ_DESCRIPTOR_CELLS
#define NJ_DESCRIPTOR_CELLS

#include "nj_descriptor.h"
#include "nj_text_buffer.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj {
#ifndef NJ_CHAR_STRUCT
#define NJ_CHAR_STRUCT

struct alignas(16) SingleCharTextureData {
    glm::vec4 uv; // 4 * 4 = 16 Bytes
};

#endif

namespace ren {

struct DescriptorCells : public DescriptorStatic {
    DescriptorCells(vk::ShaderStageFlags stages, buf::TextBufferH buf);

    virtual void CreateBuffer(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateImage(ren::DeviceH device, ren::AllocatorH allocator);
    virtual void CreateView(ren::DeviceH device, ren::AllocatorH allocator);

    void Update();

private:
    buf::TextBufferH textBuffer;
};

//
//
//
//

struct DescriptorCharactersMeta : public DescriptorStatic {
    DescriptorCharactersMeta(vk::ShaderStageFlags stages, BufferU&& buf);

    virtual void CreateBuffer(ren::DeviceH d, ren::AllocatorH a) override;
    virtual void CreateImage(ren::DeviceH d, ren::AllocatorH a) override;
    virtual void CreateView(ren::DeviceH d, ren::AllocatorH a) override;

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
