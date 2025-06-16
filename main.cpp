#include "nj_attachment_color.h"
#include "nj_grid_render_pass.h"
#include "nj_vk_build.h"
#include "njlog.h"
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

    auto inst = std::make_shared<ren::Instance>(win_ext);
    auto messenger = build::Build<vk::DebugUtilsMessengerEXT>(inst->Handle());
    auto surface = win->CreateSurface(inst->Handle());
    auto physical_device = std::make_shared<ren::PhysicalDevice>(inst);
    physical_device->UpdateQueueIndices(surface);

    auto device = std::make_shared<ren::Device>(inst, physical_device, surface);
    auto swapchain = std::make_shared<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        vk::Extent2D{ 800, 600 } 
    );
    nj::log::Info("Current swapchain extent: {}, {}", 
                  swapchain->Extent().width,
                  swapchain->Extent().height);

    auto color_att = ren::AttachmentColor(swapchain);
    auto render_pass = ren::GridRenderPass(device, color_att);


    // auto context = nj::build::Build<ren::RenderContext>(device, swapchain);

    return 0;
}
