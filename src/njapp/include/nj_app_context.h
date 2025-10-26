#pragma once
#ifndef NJ_APP_CONTEXT_H
#define NJ_APP_CONTEXT_H

#include "nj_app_handles.h"

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
    ren::DescriptorContextH expDescContext;

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
