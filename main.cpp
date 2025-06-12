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

    auto inst = build::Build<ren::Instance>(win_ext);
    auto messenger = build::Build<vk::DebugUtilsMessengerEXT>(inst->Handle());
    auto surface = win->CreateSurface(inst->Handle());
    auto physical_device = build::Build<ren::PhysicalDevice>(inst->Handle());
    physical_device->UpdateQueueIndices(surface);

    auto device = nj::build::Build<ren::Device>(inst, physical_device, surface);
    auto swapchain = nj::build::Build<ren::Swapchain>(
        physical_device, 
        device,
        surface,
        800, 600
    );
    nj::log::Info("Current swapchain extent: {}, {}", 
                  swapchain->Extent().width,
                  swapchain->Extent().height);

    auto context = nj::build::Build<ren::RenderContext>(device, swapchain);

    return 0;
}
