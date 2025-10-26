#include "nj_app_desc.h"
#include "nj_app_context.h"
#include "nj_descriptor_cells.h"
#include "nj_descriptor_context.h"
#include "nj_descriptor_grid.h"
#include "nj_descriptor_texture.h"
#include "nj_ft_atlas.h"
#include "nj_render_context.h"
#include "njcon.h"
#include <vulkan/vulkan_enums.hpp>

namespace nj::app {

auto CE(Layout l) { return static_cast<size_t>(l); }

void CreateBasicDescriptors(Context* ctx) {
    // ctx->descContext->Add<ren::DescriptorGrid>(
    //     /* Desc count */ con::Frames(),
    //     /* Layout     */ CE(Layout::Basic),
    //     /* Binding    */ 0
    // );
}

ren::BufferU CreatePageTextureBuffer(Context* ctx, size_t w, size_t h) {
    const size_t buf_sz{w * h};

    auto atlas_buf = std::make_unique<ren::Buffer>(
        ctx->device, ctx->allocator, buf_sz,
        vk::BufferUsageFlagBits::eTransferSrc,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
            | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
    );

    void* data = atlas_buf->Map();
    {
        ctx->atlasPage->MapTexture(data);
    }
    atlas_buf->Unmap();
    return atlas_buf;
}

ren::BufferU CreatePageMapBuffer(Context* ctx) {
    auto chars_buf = ren::CreateCharactersMetaBuffer(
        ctx->device, ctx->allocator, ctx->atlasPage->CodesCount()
    );

    void* data = chars_buf->Map();
    {
        ctx->atlasPage->MapMap(data);
    }
    chars_buf->Unmap();

    return chars_buf;
}

// clang-format off
void CreateAtlasPagesDescriptors(Context* ctx) {
    // const size_t PAGE_H = 1024, PAGE_W = 1024;
    //
    // ctx->descContext->Add<ren::DescriptorCells>(
    //     /* Desc count */ con::Frames(), 
    //     /* Layout     */ CE(Layout::AtlasPages), 
    //     /* Binding    */ 0, 
    //     ctx->textBuffer
    // );
    //
    // ctx->descContext->Add<ren::DescriptorTexture>(
    //     /* Desc count */ 1, 
    //     /* Layout     */ CE(Layout::AtlasPages), 
    //     /* Binding    */ 1, 
    //     vk::ShaderStageFlags(
    //         vk::ShaderStageFlagBits::eFragment | 
    //         vk::ShaderStageFlagBits::eVertex
    //     ),
    //     ctx->renderContext->CurrentCommandBuffer(), 
    //     ctx->phDevice, 
    //     ctx->sampler,
    //     PAGE_W, 
    //     PAGE_H, 
    //     CreatePageTextureBuffer(ctx, PAGE_W, PAGE_H)
    // );
    //
    // 
    // ctx->descContext->Add<ren::DescriptorCharactersMeta>(
    //     /* Desc count */ 1, 
    //     /* Layout     */ CE(Layout::AtlasPages), 
    //     /* Binding    */ 2, 
    //     CreatePageMapBuffer(ctx)
    // );
}
// clang-format on

} // namespace nj::app
