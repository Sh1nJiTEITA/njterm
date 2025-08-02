#include "nj_allocator.h"
#include "nj_attachment_color.h"
#include "nj_buffer.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_descriptor.h"
#include "nj_descriptor_context.h"
#include "nj_grid_render_pass.h"
#include "nj_render_context.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"
#include "src/njcon/njcon.h"
#include <cstdint>
#include <limits>
#include <vulkan/vulkan.hpp>
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

int main(int argc, char **argv) {
    // clang-format off
    using namespace nj;

    auto win = win::CreateWindow();
    auto win_ext = win->VulkanExtensions();

    auto inst = log::MakeSharedWithLog<ren::Instance>(win_ext);
    auto messenger = log::MakeSharedWithLog<ren::DebugUtilsMessenger>(inst);
    auto surface = win->CreateSurface(inst->Handle());

    auto physical_device = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    physical_device->UpdateQueueIndices(surface);
    physical_device->UpdateQueueProperties();

    auto device = log::MakeSharedWithLog<ren::Device>(inst, physical_device);
    physical_device->UpdateQueues(device->Handle());

    auto allocator = log::MakeSharedWithLog<ren::Allocator>("Allocator", inst, device, physical_device);

    auto swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        vk::Extent2D{ 800, 600 },
        vk::ImageUsageFlagBits::eColorAttachment
    );
    swapchain->UpdateImages(device);
    nj::log::Info("Current swapchain extent: {}, {}", 
                  swapchain->Extent().width,
                  swapchain->Extent().height);

    auto color_att = log::MakeSharedWithLog<ren::AttachmentColor>(
        "Color attachment", swapchain
    );
    auto render_pass = log::MakeSharedWithLog<ren::GridRenderPass>(device, color_att);
    auto command_pool = log::MakeSharedWithLog<ren::CommandPool>(device, physical_device);
    
    const size_t frames = con::Frames();

    auto render_context = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, render_pass, command_pool,
        frames,
        std::vector< ren::AttachmentH > { color_att } 
    );
    auto desc_pool = log::MakeSharedWithLog<ren::DescriptorPool>(device);
    

    auto desc_context = log::MakeSharedWithLog<ren::DescriptorContext>("Descriptor context", device, desc_pool, allocator, frames);

    uint32_t frame = 0;
    uint32_t current_image = 0;
    
    while (!win->ShouldClose()) {
        win->Update(); 

        auto  cdevice = device->CHandle();
        auto& frame_context = render_context->Context(frame);
        auto& sync_data = frame_context->syncData;
        auto& framebuffer = frame_context->framebuffer;
        auto  command_buffer = frame_context->commandBuffer->CHandle();
        

        auto [res_ac, _] = cdevice.acquireNextImageKHR(
            swapchain->CHandle(),
            std::numeric_limits<uint64_t>::max(), 
            frame_context->syncData->availableSemaphore.get(), 
            {}
        );
        
        if (res_ac == vk::Result::eErrorOutOfDateKHR) { 
            log::Error("Need to recreate swachain...");
        } else if (res_ac != vk::Result::eSuccess && res_ac != vk::Result::eSuboptimalKHR) { 
            log::FatalExit("Failed to acquire swapchain image...");
        }

        cdevice.resetFences(std::vector{sync_data->frameFence.get()});
        command_buffer.reset();
        
        auto command_buffer_begin_info = vk::CommandBufferBeginInfo{} 
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
            ;
        
        command_buffer.begin(command_buffer_begin_info);
        { 
            auto render_area = vk::Rect2D{}
                .setOffset({0, 0})
                .setExtent(swapchain->Extent())
                ;
            auto clear_color = vk::ClearValue{}
                .setColor({0.1f, 0.21f, 0.0f, 1.0f})
                ;
            auto render_pass_info = vk::RenderPassBeginInfo{}
                .setRenderPass(render_pass->Handle().get())
                .setFramebuffer(framebuffer->Handle().get())
                .setRenderArea(render_area)
                .setClearValues(clear_color)
                ;
            command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
            { 
                
            }
            command_buffer.endRenderPass();
        }
        command_buffer.end();


        // ---------
        
        auto wait_semaphores = std::array { sync_data->availableSemaphore.get() };
        auto wait_stages = vk::PipelineStageFlags { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        auto submit_command_buffers = std::array { command_buffer };
        auto signal_semaphores = std::array { sync_data->finishSemaphore.get() };
        auto swapchains = std::array { swapchain->CHandle() };
        auto images = std::array { current_image };

        auto submit_info = vk::SubmitInfo{}
            .setWaitSemaphores(wait_semaphores)
            .setWaitDstStageMask(wait_stages)
            .setCommandBuffers(submit_command_buffers) 
            .setSignalSemaphores(signal_semaphores)
            ;

        auto& graphics_queue = physical_device->Queue(vk::QueueFlagBits::eGraphics);
        graphics_queue.submit(submit_info, sync_data->frameFence.get());

        auto present_info = vk::PresentInfoKHR{}
            .setWaitSemaphores(signal_semaphores)
            .setSwapchains(swapchains)
            .setImageIndices(images)
            ;

        auto res_qp = graphics_queue.presentKHR(present_info);
        if (res_qp == vk::Result::eErrorOutOfDateKHR || res_qp == vk::Result::eSuboptimalKHR) {
            log::Error("Need to recreate swachain 2...");
        } else if (res_qp != vk::Result::eSuccess) { 
            log::FatalExit("Failed to present KHR...");
        }
        auto res_wf = cdevice.waitForFences(std::array {sync_data->frameFence.get() }, true, std::numeric_limits<uint64_t>::max());
        frame = (frame + 1) % frames;
    }


    render_context->CleanUp();

    return 0;
}
