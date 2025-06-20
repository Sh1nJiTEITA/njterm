#include "nj_attachment_color.h"
#include "nj_command_pool.h"
#include "nj_grid_render_pass.h"
#include "nj_render_context.h"
#include "nj_vk_build.h"
#include "njlog.h"
#include "njvklog.h"
#include "njwin.h"
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

    // auto inst = std::make_shared<ren::Instance>(win_ext);
    auto inst = log::MakeSharedWithLog<ren::Instance>(win_ext);
    auto messenger = build::Build<vk::DebugUtilsMessengerEXT>(inst->Handle());
    auto surface = win->CreateSurface(inst->Handle());
    auto physical_device = log::MakeSharedWithLog<ren::PhysicalDevice>(inst);
    physical_device->UpdateQueueIndices(surface);

    auto device = log::MakeSharedWithLog<ren::Device>(inst, physical_device, surface);
    auto swapchain = log::MakeSharedWithLog<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        vk::Extent2D{ 800, 600 } 
    );
    swapchain->UpdateImages(device);
    nj::log::Info("Current swapchain extent: {}, {}", 
                  swapchain->Extent().width,
                  swapchain->Extent().height);

    auto color_att = log::MakeSharedWithLog<ren::AttachmentColor>(
        "Color attachment", swapchain
    );
    auto render_pass = log::MakeSharedWithLog<ren::GridRenderPass>(device, *color_att);

    std::vector<ren::AttachmentH> attachments { 
        color_att
    };
    auto render_context = log::MakeSharedWithLog<ren::RenderContext>(
        "Render context", device, swapchain, attachments 
    );
    auto command_pool = log::MakeSharedWithLog<ren::CommandPool>(device, physical_device);


    // auto context = nj::build::Build<ren::RenderContext>(device, swapchain);

    return 0;
}
