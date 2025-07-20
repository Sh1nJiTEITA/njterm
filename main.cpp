#include "nj_allocator.h"
#include "nj_attachment_color.h"
#include "nj_buffer.h"
#include "nj_command_pool.h"
#include "nj_debug_utils_messenger.h"
#include "nj_grid_render_pass.h"
#include "nj_render_context.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"
#include "src/njcon/njcon.h"
#include <vulkan/vulkan_enums.hpp>

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
    auto render_context = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, render_pass, command_pool,
        con::Buffering(),
        std::vector< ren::AttachmentH > { color_att } 
    );


    ren::Buffer buf( device, allocator, 2048, vk::BufferUsageFlagBits::eVertexBuffer, {}, {});
    ren::Buffer buf1( device, allocator, 2048, vk::BufferUsageFlagBits::eVertexBuffer, {}, {});
    ren::Buffer buf3( device, allocator, 2048, vk::BufferUsageFlagBits::eVertexBuffer, {}, {}); 

    render_context->CleanUp();
    return 0;
}
