#ifndef NJ_VK_HANDLES_H
#define NJ_VK_HANDLES_H

#include <memory>
// clang-format off

#define NJ_HANDLE(NAME) class NAME; using NAME##H = std::shared_ptr<NAME>;
namespace nj::ren {
    NJ_HANDLE( Instance );
    NJ_HANDLE( DebugUtilsMessenger );
    NJ_HANDLE( Surface );
    NJ_HANDLE( PhysicalDevice );
    NJ_HANDLE( Device );
    NJ_HANDLE( Allocator );
    NJ_HANDLE( Swapchain );
    NJ_HANDLE( AttachmentColor );
    NJ_HANDLE( GridRenderPass );
    NJ_HANDLE( CommandPool );
    NJ_HANDLE( CommandBuffer );
    NJ_HANDLE( RenderContext );
    NJ_HANDLE( Sampler );
    NJ_HANDLE( DescriptorPool );
    NJ_HANDLE( DescriptorSet );
    NJ_HANDLE( DescriptorContext );
    NJ_HANDLE( DescriptorTest );
    NJ_HANDLE( DescriptorTexture );
    NJ_HANDLE( DescriptorTextureArray );
    NJ_HANDLE( PipelineBuilderBase );
    NJ_HANDLE( Pipeline );
};

#undef NJ_HANDLE


#endif
