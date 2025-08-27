#include "nj_app_context.h"
#include "njcon.h"
#include "njvklog.h"
#include "njwin.h"

#include "nj_allocator.h"
#include "nj_attachment_color.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_descriptor_test.h"
#include "nj_device.h"
#include "nj_grid_render_pass.h"
#include "nj_instance.h"
#include "nj_physical_device.h"
#include "nj_pipeline.h"
#include "nj_render_context.h"
#include "nj_sampler.h"
#include "nj_surface.h"
#include "nj_swapchain.h"

namespace nj::app {

// clang-format off
Context::Context() {
    win = win::CreateWindow({400, 500});
    InitBaseHandles();
    InitPresentHandles();
    InitDescHandles(); 
    InitPipelineHandles();
}

void Context::Run() { 
    auto clear_color = vk::ClearValue { vk::ClearColorValue{0.2f, 0.2f, 0.2f, 0.2f} } ;
    auto vertex_buffer = log::MakeSharedWithLog<ren::Buffer>(
        device, allocator, static_cast<size_t>(200),
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eVertexBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VmaAllocationCreateFlags{}
    );
    device->Handle().waitIdle();
    log::Info("============== Render loop... STARTED ==============");
    while (!win->ShouldClose()) {
        win->Update();
        if (renderContext->BeginFrame(device, swapchain)) {
            // recreate_swapchain(inst, device, phDevice, surface,
            //                    swapchain, *win, color_att, render_context,
            //                    command_pool, render_pass);
            continue;
        }
        {
            auto command_buffer = renderContext->CurrentCommandBuffer();
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
            command_buffer->Handle().beginRenderPass(render_pass_info,
            vk::SubpassContents::eInline); { // clang-format off
                descContext->BindSets(0, renderContext->CurrentFrameIndex(), command_buffer,
                pipeline->LayoutHandle());

                auto viewport = vk::Viewport{}
                                    .setX(0)
                                    .setY(0)
                                    .setHeight(swapchain->Extent().height)
                                    .setWidth(swapchain->Extent().width);

                command_buffer->Handle().setViewport(0, 1, &viewport);

                vk::Rect2D scissor{};
                scissor.offset = vk::Offset2D{static_cast<int32_t>(0),
                static_cast<int32_t>(0)}; scissor.extent = vk::Extent2D{
                    static_cast<uint32_t>(swapchain->Extent().width),
                    static_cast<uint32_t>(swapchain->Extent().height)
                };
                command_buffer->Handle().setScissor(0, 1, &scissor);

                auto buffers = std::array<vk::Buffer, 1>{
                vertex_buffer->CHandle() }; auto offsets =
                std::array<vk::DeviceSize, 1>{ {} };

                command_buffer->Handle().bindVertexBuffers(
                    0, buffers, offsets
                );

                command_buffer->Handle().bindPipeline(
                vk::PipelineBindPoint::eGraphics, pipeline->CHandle());
                command_buffer->Handle().draw(6, 1, 0, 0);
            } // clang-format on:
            command_buffer->Handle().endRenderPass();
        }
        if (renderContext->EndFrame(device, phDevice, swapchain)) {
            // recreate_swapchain(inst, device, physical_device, surface,
            //                    swapchain, *win, color_att, render_context,
            //                    command_pool, render_pass);
        }
    }
    log::Info("============== Render loop... ENDED   ==============");
    clear_color = {};
    device->Handle().waitIdle();
}

void Context::InitBaseHandles() {
    inst = log::MakeSharedWithLog<ren::Instance>(win->VulkanExtensions());
    messenger = log::MakeSharedWithLog<ren::DebugUtilsMessenger>(inst);
    surface = log::MakeSharedWithLog<ren::Surface>(
        inst, win->CreateSurface(inst->Handle()));
    phDevice = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    phDevice->UpdateQueueIndices(surface);
    phDevice->UpdateQueueProperties();
    device = log::MakeSharedWithLog<ren::Device>(inst, phDevice);
    phDevice->UpdateQueues(device->Handle());
    allocator = log::MakeSharedWithLog<ren::Allocator>(inst, device, phDevice);
}

void Context::InitPresentHandles() {
    swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        phDevice, device, surface,
        vk::Extent2D{static_cast<uint32_t>(win->Extent().x),
                     static_cast<uint32_t>(win->Extent().y)},
        vk::ImageUsageFlagBits::eColorAttachment);
    swapchain->UpdateImages(device);
    attColor = log::MakeSharedWithLog<ren::AttachmentColor>("Color attachment",
                                                            swapchain);
    gridRenderPass =
        log::MakeSharedWithLog<ren::GridRenderPass>(device, attColor);
    cmdPool = log::MakeSharedWithLog<ren::CommandPool>(device, phDevice);
    renderContext = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, gridRenderPass, cmdPool,
        con::Frames(), con::FrameObjectsMode(),
        std::vector<ren::AttachmentH>{attColor});
}

void Context::InitDescHandles() {
    sampler = log::MakeSharedWithLog<ren::Sampler>(device);
    descPool = log::MakeSharedWithLog<ren::DescriptorPool>(device);
    descContext = log::MakeSharedWithLog<ren::DescriptorContext>(
        "Descriptor context", device, descPool, allocator, con::Frames());
    descContext->Add<ren::DescriptorTest>(con::Frames(), 0, 0);
    descContext->CreateLayouts();
    descContext->AllocateSets();
    descContext->UpdateSets();
}

void Context::InitPipelineHandles() {
    auto all_layouts = descContext->AllLayouts();
    log::Debug("Descriptor layout count={}", all_layouts.size());
    pipelineBuilderTest =
        log::MakeSharedWithLog<ren::PipelineBuilderTest>("PipelineBuilderTest");
    pipeline = log::MakeSharedWithLog<ren::Pipeline>(
        device, gridRenderPass, pipelineBuilderTest, all_layouts,
        fs::path("/home/snj/Code/Other/njterm/build/shaders/basic/"));
}

// clang-format on

}; // namespace nj::app
