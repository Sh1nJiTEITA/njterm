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
    NJ_HANDLE( AtlasPage );
}
namespace nj::buf {
    NJ_HANDLE( TextBuffer );
}

#undef NJ_HANDLE

// clang-format on

namespace nj::app {

struct Context {
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

    ren::PipelineH pipeline;
    ren::PipelineH guidelinesPipeline;
    ren::PipelineH cellsPipeline;

    buf::TextBufferH textBuffer;

    ft::LibraryH library;
    ft::AtlasPageH atlasPage;
    ft::FaceH face;
};

struct App : private Context {
    App();

    void Run();
    void Update();

private:
    //                               Initialize
    // ========================================================================
    void InitBaseHandles();
    void InitPresentHandles();
    void InitDescriptorHandles();
    void InitPipelineHandles();
    void InitFontLoaderHandles();
    void InitTextBuffer();

    //                                Updates
    // ========================================================================
    void UpdateWindow();
    void UpdateDescriptors();

    //                                 Other
    // ========================================================================
    void RecreateSwapchain();
};

}; // namespace nj::app

#endif
