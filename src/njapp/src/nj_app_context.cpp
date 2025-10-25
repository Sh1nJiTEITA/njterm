#include "nj_app_context.h"
#include "nj_app_desc.h"
#include "nj_pipeline_cells.h"
#include "nj_pipeline_guidelines.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"

#include "nj_allocator.h"
#include "nj_attachment_color.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_descriptor.h"
#include "nj_descriptor_cells.h"
#include "nj_descriptor_context.h"
#include "nj_descriptor_grid.h"
#include "nj_descriptor_test.h"
#include "nj_descriptor_texture.h"
#include "nj_device.h"
#include "nj_ft_atlas.h"
#include "nj_ft_library.h"
#include "nj_grid_render_pass.h"
#include "nj_instance.h"
#include "nj_physical_device.h"
#include "nj_pipeline.h"
#include "nj_render_context.h"
#include "nj_sampler.h"
#include "nj_surface.h"
#include "nj_swapchain.h"
#include "nj_text_buffer.h"
#include <algorithm>
#include <glm/fwd.hpp>
#include <iterator>
#include <memory>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::app {

// clang-format off
App::App() {
    win = win::CreateWindow({400, 1440});
    InitFontLoaderHandles();
    
    InitBaseHandles();
    InitPresentHandles();
    InitTextBuffer();
    InitDescriptorHandles(); 
    InitExpDescriptorHandles();
    InitPipelineHandles();
}

void App::Run() { 

    // ft::AtlasPage page{ 
    //     face, 0, 30, 512, 512, 32, 255
    // };
    // page.SaveToBMP("./test.bmp");

    auto clear_color = vk::ClearValue { vk::ClearColorValue{0.2f, 0.2f, 0.2f, 0.2f} } ;
    device->Handle().waitIdle();
    log::Info("============== Render loop... STARTED ==============");
    while (!win->ShouldClose()) {
        Update();
        if (renderContext->BeginFrame(device, swapchain)) {
            RecreateSwapchain();
            continue;
        }
        {
            auto cmd = renderContext->CurrentCommandBuffer();
            auto render_area = vk::Rect2D{}
                .setOffset({0, 0})
                .setExtent(swapchain->Extent())
                ;

            auto render_pass_info = vk::RenderPassBeginInfo{}
                .setRenderPass(gridRenderPass->Handle())
                .setFramebuffer(renderContext->CurrentFramebuffer()->Handle())
                .setRenderArea(render_area)
                .setClearValues(clear_color)
                ;
            cmd->Handle().beginRenderPass(render_pass_info, vk::SubpassContents::eInline); { 
                // clang-format off

                auto viewport = vk::Viewport{}
                                    .setX(0)
                                    .setY(0)
                                    .setHeight(swapchain->Extent().height)
                                    .setWidth(swapchain->Extent().width);

                cmd->Handle().setViewport(0, 1, &viewport);

                vk::Rect2D scissor{};
                scissor.offset = vk::Offset2D{static_cast<int32_t>(0),
                static_cast<int32_t>(0)}; scissor.extent = vk::Extent2D{
                    static_cast<uint32_t>(swapchain->Extent().width),
                    static_cast<uint32_t>(swapchain->Extent().height)
                };
                cmd->Handle().setScissor(0, 1, &scissor);

                // descContext->BindSets(0, renderContext->CurrentFrameIndex(), cmd, guidelinesPipeline->LayoutHandle());
                expDescContext->Bind(0, renderContext->CurrentFrameIndex(), cmd, guidelinesPipeline->LayoutHandle());
                descContext->BindSets(1, renderContext->CurrentFrameIndex(), cmd, guidelinesPipeline->LayoutHandle());
                gridRenderPass->RenderGuidelines(cmd, guidelinesPipeline);

                // descContext->BindSets(0, renderContext->CurrentFrameIndex(), cmd, cellsPipeline->LayoutHandle());
                expDescContext->Bind(0, renderContext->CurrentFrameIndex(), cmd, cellsPipeline->LayoutHandle());
                descContext->BindSets(1, renderContext->CurrentFrameIndex(), cmd, cellsPipeline->LayoutHandle());
                gridRenderPass->RenderCells(cmd, cellsPipeline);
            } // clang-format on:
            cmd->Handle().endRenderPass();
            if (renderContext->EndFrame(device, phDevice, swapchain)) {
                RecreateSwapchain();
            }
        }
    }
    log::Info("============== Render loop... ENDED   ==============");
    clear_color = {};
    device->Handle().waitIdle();
}

void App::Update() {

    UpdateWindow();
    UpdateDescriptors();
}

void App::InitBaseHandles() {
    inst = log::MakeSharedWithLog<ren::Instance>(win->VulkanExtensions());

    messenger = log::MakeSharedWithLog<ren::DebugUtilsMessenger>(inst);

    surface = log::MakeSharedWithLog<ren::Surface>(
        inst, win->CreateSurface(inst->Handle())
    );

    phDevice = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    phDevice->UpdateQueueIndices(surface);
    phDevice->UpdateQueueProperties();

    device = log::MakeSharedWithLog<ren::Device>(inst, phDevice);
    phDevice->UpdateQueues(device->Handle());

    allocator = log::MakeSharedWithLog<ren::Allocator>(inst, device, phDevice);
}

void App::InitPresentHandles() {
    swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        phDevice, device, surface,
        vk::Extent2D{
            static_cast<uint32_t>(win->Extent().x),
            static_cast<uint32_t>(win->Extent().y)
        },
        vk::ImageUsageFlagBits::eColorAttachment
    );
    swapchain->UpdateImages(device);

    attColor = log::MakeSharedWithLog<ren::AttachmentColor>(
        "Color attachment", swapchain
    );

    gridRenderPass =
        log::MakeSharedWithLog<ren::GridRenderPass>(device, attColor);

    cmdPool = log::MakeSharedWithLog<ren::CommandPool>(device, phDevice);

    renderContext = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, gridRenderPass, cmdPool,
        con::Frames(), std::vector<ren::AttachmentH>{attColor}
    );
}

void App::InitDescriptorHandles() {
    sampler = log::MakeSharedWithLog<ren::Sampler>(device);
    descPool = log::MakeSharedWithLog<ren::DescriptorPool>(device);
    descContext = log::MakeSharedWithLog<ren::DescriptorContext>(
        "Descriptor context", device, descPool, allocator, con::Frames()
    );

    CreateBasicDescriptors(this);
    CreateAtlasPagesDescriptors(this);

    descContext->CreateLayouts();
    descContext->AllocateSets();
    descContext->UpdateSets();
}

void App::InitExpDescriptorHandles() {
    expDescContext = log::MakeSharedWithLog<ren::exp::DescriptorContext>("123");

    auto set_0 = std::make_unique<ren::exp::DescriptorSet>(con::Frames());
    set_0->RegisterPerFrame<ren::exp::DescriptorGrid>(
        0, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
    );
    set_0->InitializeDescriptors(device, allocator);
    expDescContext->Add(0, std::move(set_0));

    expDescContext->Create(device);
    expDescContext->Allocate(device, descPool);
    expDescContext->Update(device);
}

void App::InitPipelineHandles() {
    auto def_all_layouts = descContext->AllLayouts();
    auto exp_all_layouts = expDescContext->AllLayouts();
    auto all_layouts = decltype(def_all_layouts){};

    std::merge(
        def_all_layouts.begin(), def_all_layouts.end(), exp_all_layouts.begin(),
        exp_all_layouts.end(), std::back_inserter(all_layouts)
    );

    log::Debug("Descriptor layout count={}", all_layouts.size());

    // pipeline = ren::CreatePipeline<ren::PipelineBuilderBase>(
    //     device, gridRenderPass, all_layouts,
    //     fs::path("/home/snj/Code/Other/njterm/build/shaders/basic/")
    // );

    guidelinesPipeline = ren::CreatePipeline<ren::PipelineBuilderGuidelines>(
        device, gridRenderPass, all_layouts,
        fs::path("/home/snj/Code/Other/njterm/build/shaders/guidelines/")
    );

    cellsPipeline = ren::CreatePipeline<ren::PipelineBuilderCells>(
        device, gridRenderPass, all_layouts,
        fs::path("/home/snj/Code/Other/njterm/build/shaders/cells/")
    );

    gridRenderPass->CreateGuidelinesBuffer(
        device, allocator,
        glm::ivec2{swapchain->Extent().width, swapchain->Extent().height},
        atlasPage->Box()
    );

    log::Debug("faceBox={},{}", atlasPage->Box().x, atlasPage->Box().y);
    gridRenderPass->CreateCellsBuffer(
        device, allocator, textBuffer->Size(),
        glm::ivec2{swapchain->Extent().width, swapchain->Extent().height},
        atlasPage->Box()
    );
    log::Debug("faceBox={},{}", atlasPage->Box().x, atlasPage->Box().y);
}

void App::InitFontLoaderHandles() {
    const fs::path font_path{
        "/usr/share/fonts/TTF/0xProtoNerdFontPropo-Regular.ttf"
    };
    library = std::make_shared<ft::Library>();
    ft::FaceID id = library->LoadFace(font_path);
    face = library->GetFace(id);
    atlasPage = std::make_shared<ft::AtlasPage>(face, 0, 30, 512, 512, 32, 255);
    // atlas = std::make_shared<ft::Atlas>(face, 0, 150, 32, 255);
}

void App::InitTextBuffer() {
    textBuffer = log::MakeSharedWithLog<buf::TextBuffer>(
        "Text buffer", con::TextBufferSize()
    );
    textBuffer->FillWithRainbow();
}

void App::UpdateWindow() { win->Update(); }

// clang-format off
void App::UpdateDescriptors() {
    const auto frame = renderContext->CurrentFrameIndex();

    expDescContext->GetDescriptor<ren::exp::DescriptorGrid>(Layout::Basic, 0, frame)
    .Update(
        swapchain->ExtentPixels(),
        atlasPage->Box(), 
        atlasPage->PageSize()
    );
    
    descContext->Get<ren::DescriptorCells>(frame, Layout::AtlasPages, 0).Update();
}
// clang-format on

void App::RecreateSwapchain() {
    log::Debug("Recreating swapchain ... STARTED");
    log::Debug("Waiting... STARTED");
    win->WaitToRecreate();
    device->Handle().waitIdle();
    renderContext->ClearFramebuffers();
    renderContext->ClearSyncDatas();
    renderContext->ClearCmds();
    attColor.reset();
    swapchain.reset();
    gridRenderPass->DestroyGuidelinesBuffer();
    auto extent = win->Extent();
    auto vk_extent = vk::Extent2D{
        static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y)
    };
    swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        phDevice, device, surface, vk_extent,
        vk::ImageUsageFlagBits::eColorAttachment
    );
    swapchain->UpdateImages(device);
    attColor = log::MakeSharedWithLog<ren::AttachmentColor>(
        "Color attachment", swapchain
    );
    renderContext->CreateFramebuffers(
        device, swapchain, gridRenderPass, {attColor}
    );
    renderContext->CreateSyncDatas(device);
    renderContext->CreateCmds(device, cmdPool);
    gridRenderPass->CreateGuidelinesBuffer(
        device, allocator,
        glm::ivec2{swapchain->Extent().width, swapchain->Extent().height},
        atlasPage->Box()
    );
    log::Debug("Recreating swapchain ... DONE");
}

// clang-format on

}; // namespace nj::app
