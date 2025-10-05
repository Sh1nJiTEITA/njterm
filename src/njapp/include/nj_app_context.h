#pragma once
#ifndef NJ_APP_CONTEXT_H
#define NJ_APP_CONTEXT_H

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
    NJ_HANDLE( RenderContext );
    NJ_HANDLE( Sampler );
    NJ_HANDLE( DescriptorPool );
    NJ_HANDLE( DescriptorContext );
    NJ_HANDLE( DescriptorTest );
    NJ_HANDLE( DescriptorTexture );
    NJ_HANDLE( PipelineBuilderBase );
    NJ_HANDLE( Pipeline );
};
namespace nj::win {
    NJ_HANDLE( Window );
    NJ_HANDLE( KeyControl );
}
namespace nj::ft { 
    NJ_HANDLE( Library );
    NJ_HANDLE( Atlas );
    NJ_HANDLE( Face );
}

#undef NJ_HANDLE

// clang-format on

namespace nj::app {

class Context {
public:
    Context();
    void Run();
    void Update();

private:
    void InitBaseHandles();
    void InitPresentHandles();
    void InitDescHandles();
    void InitPipelineHandles();
    void InitFontLoaderHandles();

    void RecreateSwapchain();

private:
    win::WindowH win;
    win::KeyControlH keyCtl;
    ren::InstanceH inst;
    ren::DebugUtilsMessengerH messenger;
    ren::SurfaceH surface;
    ren::PhysicalDeviceH phDevice;
    ren::DeviceH device;
    ren::AllocatorH allocator;
    ren::SwapchainH swapchain;
    ren::AttachmentColorH attColor;
    ren::GridRenderPassH gridRenderPass;
    ren::CommandPoolH cmdPool;
    ren::RenderContextH renderContext;
    ren::SamplerH sampler;
    ren::DescriptorPoolH descPool;
    ren::DescriptorContextH descContext;
    ren::DescriptorTestH descTest;
    ren::DescriptorTextureH descTexture;
    // ren::PipelineBuilderBaseH pipelineBuilder;
    ren::PipelineH pipeline;
    ren::PipelineH guidelinesPipeline;

    ft::LibraryH library;
    ft::AtlasH atlas;
    ft::FaceH face;
};

}; // namespace nj::app

#endif
