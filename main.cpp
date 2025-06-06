#include "nj_vk_build.h"
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
    auto win = nj::win::CreateWindow();
    auto win_ext = win->VulkanExtensions();

    auto inst = nj::build::Build<vk::Instance>(win_ext);
    auto messenger = nj::build::Build<vk::DebugUtilsMessengerEXT>(inst);
    auto surface = win->CreateSurface(inst);
    auto physical_device = nj::build::Build<vk::PhysicalDevice>(inst);
    auto device = nj::build::Build<vk::Device>(inst, physical_device, surface);

    return 0;
}
