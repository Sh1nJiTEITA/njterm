#include "nj_allocator.h"
#include "nj_attachment_color.h"
#include "nj_buffer.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_ft_library.h"
#include "nj_grid_render_pass.h"
#include "nj_pipeline.h"
#include "nj_render_context.h"
#include "njcon.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"

#include <cstdint>
#include <limits>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

/*

    // auto keyctrl = nj::win::CreateKeyControl();
    // nj::Pty p;
    // p.ConnectShell();
    // p.ReadMaster(1, 50);
    // p.WriteMaster("ls\n");
    // p.ReadMaster(1, 100);

*/

void load_fonts() {
    using namespace nj;
    ft::Library lib{};
    fs::path font_path{"/usr/share/fonts/TTF/0xProtoNerdFontPropo-Regular.ttf"};
    ft::FaceID id = lib.LoadFace(font_path);
};

int main(int argc, char **argv) {
    // clang-format off
    using namespace nj;

    load_fonts(); 

    
    auto win = win::CreateWindow();
    auto win_ext = win->VulkanExtensions();

    auto inst = log::MakeSharedWithLog<ren::Instance>(win_ext);
    auto messenger = log::MakeSharedWithLog<ren::DebugUtilsMessenger>(inst);
    auto surface = win->CreateSurface(inst->Handle());
    //
    auto physical_device = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    physical_device->UpdateQueueIndices(surface);
    physical_device->UpdateQueueProperties();
    //
    auto device = log::MakeSharedWithLog<ren::Device>(inst, physical_device);
    physical_device->UpdateQueues(device->Handle());
    //
    auto allocator = log::MakeSharedWithLog<ren::Allocator>("Allocator", inst, device, physical_device);
    //
    auto swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        vk::Extent2D{ 800, 600 },
        vk::ImageUsageFlagBits::eColorAttachment
    );
    swapchain->UpdateImages(device);
    //
    nj::log::Info("Current swapchain extent: {}, {}", 
                  swapchain->Extent().width,
                  swapchain->Extent().height);
    //
    auto color_att = log::MakeSharedWithLog<ren::AttachmentColor>(
        "Color attachment", swapchain
    );
    auto render_pass = log::MakeSharedWithLog<ren::GridRenderPass>(device, color_att);
    auto command_pool = log::MakeSharedWithLog<ren::CommandPool>(device, physical_device);
    // 
    const size_t frames = swapchain->Images().size();

    auto render_context = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, render_pass, command_pool,
        frames,
        std::vector< ren::AttachmentH > { color_att } 
    );
    auto desc_pool = log::MakeSharedWithLog<ren::DescriptorPool>(device);
    auto desc_context = log::MakeSharedWithLog<ren::DescriptorContext>("Descriptor context", device, desc_pool, allocator, frames);

    auto pipeline_builder = log::MakeSharedWithLog<ren::PipelineBuilderTest>("PipelineBuilderTest");
    auto pipeline = log::MakeSharedWithLog<ren::Pipeline>(device, render_pass, pipeline_builder, std::vector<vk::SharedDescriptorSetLayout>{}, fs::path("/home/snj/Code/Other/njterm/build/basic/"));

    uint32_t frame = 0;
    uint32_t current_image = 0;
    
    auto clear_color = vk::ClearValue { vk::ClearColorValue{0.2f, 0.2f, 0.2f, 0.2f} } ;

    // auto vertex_buffer = 

    while (!win->ShouldClose()) {
        win->Update(); 
        // log::Debug("Frame={} Image={}", render_context->CurrentFrameIndex(), render_context->CurrentImageIndex());
        render_context->BeginFrame(device, swapchain);
        { 
            auto command_buffer = render_context->CurrentCommandBuffer();
            auto render_area = vk::Rect2D{}
                .setOffset({0, 0})
                .setExtent(swapchain->Extent())
                ;
            
            auto render_pass_info = vk::RenderPassBeginInfo{}
                .setRenderPass(render_pass->Handle().get())
                .setFramebuffer(render_context->CurrentFramebuffer()->Handle().get())
                .setRenderArea(render_area)
                .setClearValues(clear_color)
                ;
            command_buffer->Handle()->beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
            { // clang-format on
                auto viewport = vk::Viewport{}
                                    .setX(0)
                                    .setY(0)
                                    .setHeight(swapchain->Extent().height)
                                    .setWidth(swapchain->Extent().width);

                command_buffer->Handle()->setViewport(0, 1, &viewport);

                vk::Rect2D scissor{};
                scissor.offset = vk::Offset2D{static_cast<int32_t>(0),
                                              static_cast<int32_t>(0)};
                scissor.extent = vk::Extent2D{
                    static_cast<uint32_t>(swapchain->Extent().width),
                    static_cast<uint32_t>(swapchain->Extent().height)};
                command_buffer->Handle()->setScissor(0, 1, &scissor);
                command_buffer->Handle()->bindPipeline(
                    vk::PipelineBindPoint::eGraphics, pipeline->CHandle());
                // command_buffer.bindVertexBuffers(1, {}, {});
                command_buffer->Handle()->draw(3, 1, 0, 0);
            } // clang-format off
            command_buffer->Handle()->endRenderPass();
        }
        render_context->EndFrame(device, physical_device, swapchain);
    }
    clear_color = {};
    device->Handle()->waitIdle();
    log::Debug("Render loop ended");
    return 0;
}
