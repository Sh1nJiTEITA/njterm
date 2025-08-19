#include "nj_allocator.h"
#include "nj_attachment_color.h"
// #include "nj_buffer.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_descriptor_test.h"
#include "nj_descriptor_texture.h"
#include "nj_device.h"
#include "nj_ft_atlas.h"
#include "nj_ft_library.h"
#include "nj_grid_render_pass.h"
#include "nj_instance.h"
#include "nj_log_char.h"
#include "nj_physical_device.h"
#include "nj_pipeline.h"
#include "nj_render_context.h"
#include "nj_sampler.h"
#include "nj_swapchain.h"
// #include "njcon.h"
#include "nj_surface.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"
//
// #include <cstdint>
// #include <limits>
// #include <memory>
// #include <sstream>
// #include <vulkan/vulkan.hpp>
// #include <vulkan/vulkan_core.h>
// #include <vulkan/vulkan_enums.hpp>
// #include <vulkan/vulkan_handles.hpp>
// #include <vulkan/vulkan_shared.hpp>
// #include <vulkan/vulkan_structs.hpp>

using namespace nj;
/*

    // auto keyctrl = nj::win::CreateKeyControl();
    // nj::Pty p;
    // p.ConnectShell();
    // p.ReadMaster(1, 50);
    // p.WriteMaster("ls\n");
    // p.ReadMaster(1, 100);

*/

struct FtContext {
    ft::LibraryH lib;
    ft::FaceH face;
    ft::AtlasH atlas;
};

FtContext create_ft_context() {
    const fs::path font_path{
        "/usr/share/fonts/TTF/0xProtoNerdFontPropo-Regular.ttf"};
    auto lib = std::make_shared<ft::Library>();

    ft::FaceID id = lib->LoadFace(font_path);
    auto face = lib->GetFace(id);
    // auto atlas = std::make_shared<ft::Atlas>(face, 12, 12, 32, 255);
    auto atlas = std::make_shared<ft::Atlas>(face, 0, 300, 32, 255);
    return {.lib = std::move(lib),
            .face = std::move(face),
            .atlas = std::move(atlas)};
};

struct CharData {
    std::vector<uint8_t> data;
    const size_t cols;
    const size_t rows;
    const size_t stride;
};

CharData create_char_bitmap(ft::FaceH face, char ch) {
    face->SetPixelSize(800, 600);
    // face->SetPixelSize(12, 100);
    if (!face->LoadGlyph(ch)) {
        std::exit(1);
    }
    auto &bm = face->Glyph()->bitmap;
    // std::stringstream ss;
    std::vector<uint8_t> data(bm.rows * bm.width, 0);
    for (size_t i = 0; i < bm.rows; ++i) {
        for (size_t j = 0; j < bm.width; ++j) {
            const size_t index = j + i * bm.width;
            // ss << static_cast<int>(bm.buffer[index]) << " ";
            data[index] = bm.buffer[index];
        }
        // ss << "\n";
    }
    log::Debug("\n");
    log::PrintChar(data, bm.rows, bm.width);
    return {.data = std::move(data),
            .cols = bm.width,
            .rows = bm.rows,
            .stride = static_cast<size_t>(bm.pitch)};
}

const size_t ATLAS_W = 4000;
const size_t ATLAS_H = 4000;

std::unique_ptr<ren::Buffer> create_atlas_buffer(ren::DeviceH device,
                                                 ren::AllocatorH allocator) {
    const size_t buf_sz{ATLAS_W * ATLAS_H};
    return std::make_unique<ren::Buffer>(
        device, allocator, buf_sz, vk::BufferUsageFlagBits::eTransferSrc,
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
};

std::unique_ptr<ren::Buffer> upload_font(std::unique_ptr<ren::Buffer> buf,
                                         FtContext &ft) {
    void *data = buf->Map();
    ft.atlas->Upload(data, ATLAS_W, ATLAS_H);
    buf->Unmap();
    return buf;
}
//
// // clang-format off
// void recreate_swapchain(ren::InstanceH& instance,
//                         ren::DeviceH& device,
//                         ren::PhysicalDeviceH& physical_device,
//                         vk::SharedSurfaceKHR& surface,
//                         ren::SwapchainH& swapchain,
//                         win::Window &window,
//                         ren::AttachmentColorH& color_att,
//                         ren::RenderContextH& render_context,
//                         ren::CommandPoolH& command_pool,
//                         ren::RenderPassH render_pass) {
//     log::Debug("Recreating swapchain ... STARTED");
//     log::Debug("Waiting... STARTED");
//     window.WaitToRecreate();
//     device->Handle()->waitIdle();
//     render_context->WaitFences(device, swapchain);
//
//
//     log::Debug("Waiting... DONE");
//
//     log::Debug("Clearing... STARTED");
//     render_context->ClearImageContexts();
//     render_context->ClearFrameContexts();
//     color_att.reset();
//     swapchain.reset();
//     log::Debug("Clearing... DONE");
//     auto extent = window.Extent();
//     auto vk_extent = vk::Extent2D{static_cast<uint32_t>(extent.x),
//                                   static_cast<uint32_t>(extent.y)};
//     //
//     swapchain = log::MakeSharedWithLog<ren::Swapchain>(
//         physical_device, device, surface, vk_extent,
//         vk::ImageUsageFlagBits::eColorAttachment
//     );
//     swapchain->UpdateImages(device);
//
//     color_att = log::MakeSharedWithLog<ren::AttachmentColor>(
//         "Color attachment", swapchain
//     );
//
//     // render_context->CreateFrameContexts(device, command_pool,
//     swapchain->Images().size()); render_context->CreateFrameContexts(device,
//     command_pool, 2); render_context->CreateImageContexts(
//         device,
//         swapchain,
//         render_pass,
//         std::vector< ren::AttachmentH > { color_att }
//     );
//     //
//     log::Debug("Recreating swapchain ... DONE");
// }
// // clang-format on

int main(int argc, char **argv) {
    // clang-format off

    auto win = win::CreateWindow({ 800, 800 });
    auto win_extent = win->Extent();
    auto win_ext = win->VulkanExtensions();

    auto inst = log::MakeSharedWithLog<ren::Instance>(win_ext);
    auto messenger = log::MakeSharedWithLog<ren::DebugUtilsMessenger>(inst);
    auto surface = log::MakeSharedWithLog<ren::Surface>(inst, win->CreateSurface(inst->Handle()));
    // //
    auto physical_device = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    physical_device->UpdateQueueIndices(surface);
    physical_device->UpdateQueueProperties();
    // //
    auto device = log::MakeSharedWithLog<ren::Device>(inst, physical_device);
    physical_device->UpdateQueues(device->Handle());
    // //
    auto allocator = log::MakeSharedWithLog<ren::Allocator>(inst, device, physical_device);
    // //
    // 
    auto swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        vk::Extent2D{ static_cast<uint32_t>(win_extent.x), 
                      static_cast<uint32_t>(win_extent.y) },
        vk::ImageUsageFlagBits::eColorAttachment
    );
    swapchain->UpdateImages(device);
    // //
    nj::log::Info("Current swapchain extent: {}, {}", swapchain->Extent().width, swapchain->Extent().height);
    // //
    auto color_att = log::MakeSharedWithLog<ren::AttachmentColor>("Color attachment", swapchain);
    auto render_pass = log::MakeSharedWithLog<ren::GridRenderPass>(device, color_att);
    auto command_pool = log::MakeSharedWithLog<ren::CommandPool>(device, physical_device);
    // // 
    // const size_t frames = con::Frames();
    const size_t frames = 2;
    // const size_t frames = swapchain->Images().size();
    //
    auto render_context = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, render_pass, command_pool,
        frames,
        std::vector< ren::AttachmentH > { color_att } 
    );
    // 
    auto sampler = log::MakeSharedWithLog<ren::Sampler>(device);
    //
    auto desc_pool = log::MakeSharedWithLog<ren::DescriptorPool>(device);
    auto desc_context = log::MakeSharedWithLog<ren::DescriptorContext>(
        "Descriptor context", device, desc_pool, allocator, frames
    );
    // // auto desc_test = log::MakeSharedWithLog<ren::DescriptorTest>("DescriptorTest");
    desc_context->Add<ren::DescriptorTest>(frames, 0, 0);
    //
    auto ft_context = create_ft_context(); 
    auto atlas_buf = create_atlas_buffer(device, allocator);
    atlas_buf = upload_font(std::move(atlas_buf), ft_context);
    //
    //
    // auto char_bm = create_char_bitmap(ft_context.face, 'w');
    desc_context->Add<ren::DescriptorTexture>(
        1, 0, 1, 
        vk::ShaderStageFlags(vk::ShaderStageFlagBits::eFragment),
        render_context->CurrentCommandBuffer(), 
        physical_device, 
        sampler,
        ATLAS_W, ATLAS_H, std::move(atlas_buf)
        // char_bm.rows, char_bm.cols, char_bm.stride,
        // char_bm.data
        // atlas.Side(), atlas.Side(), 
        // atlas.Bitmap()
    );
    desc_context->CreateLayouts();
    desc_context->AllocateSets();
    desc_context->UpdateSets();
    auto all_layouts = desc_context->AllLayouts();
    log::Debug("Descriptor layout count={}", all_layouts.size());
    //
    auto pipeline_builder = log::MakeSharedWithLog<ren::PipelineBuilderTest>("PipelineBuilderTest");
    auto pipeline = log::MakeSharedWithLog<ren::Pipeline>(
        device, render_pass, pipeline_builder, all_layouts, 
        fs::path("/home/snj/Code/Other/njterm/build/shaders/basic/")
    );
    //
    uint32_t frame = 0;
    uint32_t current_image = 0;
    // 
    auto clear_color = vk::ClearValue { vk::ClearColorValue{0.2f, 0.2f, 0.2f, 0.2f} } ;
    //
    auto vertex_buffer = log::MakeSharedWithLog<ren::Buffer>("Buffer",
        device, allocator, static_cast<size_t>(200), 
        vk::BufferUsageFlags(vk::BufferUsageFlagBits::eVertexBuffer),
        VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
        VmaAllocationCreateFlags{}
    );
    //
    //
    log::Info("============== Render loop... STARTED ==============");
    while (!win->ShouldClose()) {
        win->Update(); 
        if (render_context->BeginFrame(device, swapchain)) { 
            // recreate_swapchain(inst, device, physical_device, surface,
            //                    swapchain, *win, color_att, render_context,
            //                    command_pool, render_pass);
            continue;
        }
        { 
            auto command_buffer = render_context->CurrentCommandBuffer();
            auto render_area = vk::Rect2D{}
                .setOffset({0, 0})
                .setExtent(swapchain->Extent())
                ;
            
            auto render_pass_info = vk::RenderPassBeginInfo{}
                .setRenderPass(render_pass->Handle())
                .setFramebuffer(render_context->CurrentFramebuffer()->Handle())
                .setRenderArea(render_area)
                .setClearValues(clear_color)
                ;
            command_buffer->Handle().beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
            { // clang-format off
                desc_context->BindSets(0, frame, command_buffer, pipeline->LayoutHandle());

                auto viewport = vk::Viewport{}
                                    .setX(0)
                                    .setY(0)
                                    .setHeight(swapchain->Extent().height)
                                    .setWidth(swapchain->Extent().width);

                command_buffer->Handle().setViewport(0, 1, &viewport);

                vk::Rect2D scissor{};
                scissor.offset = vk::Offset2D{static_cast<int32_t>(0), static_cast<int32_t>(0)};
                scissor.extent = vk::Extent2D{ 
                    static_cast<uint32_t>(swapchain->Extent().width),
                    static_cast<uint32_t>(swapchain->Extent().height)
                };
                command_buffer->Handle().setScissor(0, 1, &scissor);
                
                auto buffers = std::array<vk::Buffer, 1>{ vertex_buffer->CHandle() };
                auto offsets = std::array<vk::DeviceSize, 1>{ {} };
                
                command_buffer->Handle().bindVertexBuffers(
                    0, buffers, offsets
                );

                command_buffer->Handle().bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline->CHandle());
                command_buffer->Handle().draw(6, 1, 0, 0);
            } // clang-format on:
            command_buffer->Handle().endRenderPass();
        }
        if (render_context->EndFrame(device, physical_device, swapchain)) {
            // recreate_swapchain(inst, device, physical_device, surface,
            //                    swapchain, *win, color_att, render_context,
            //                    command_pool, render_pass);
        }
    }
    log::Info("============== Render loop... ENDED   ==============");
    //
    clear_color = {};
    device->Handle().waitIdle();
    return 0;
}
