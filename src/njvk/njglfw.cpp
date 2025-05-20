#include "njglfw.h"
#include "njlog.h"
#include "njwin.h"
#include <GLFW/glfw3.h>

namespace nj {
namespace glfw {

WindowGLFW::WindowGLFW(std::string title, glm::vec2 init_ext)
    : title{title}, extent{init_ext} {
    glfwInit();
    const auto display_server = win::DisplayServer();
    log::CheckCall(display_server,
                   "Cant create GLFW window: Unknown compositor");
    // FIXME: Add support for other compositors
    if (display_server) {
        glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    } else {
        glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
}

WindowGLFW::~WindowGLFW() {
    if (glfwWindowHandle) {
        glfwDestroyWindow(glfwWindowHandle);
    }
}

auto WindowGLFW::Create() -> void {
    glfwWindowHandle =
        glfwCreateWindow(extent.x, extent.y, title.c_str(), nullptr, nullptr);
    // glfwSetFramebufferSizeCallback(glfwWindow, GLFWframebuffersizefun
    // callback)
}

auto WindowGLFW::Extent() -> glm::vec2 {}
auto WindowGLFW::Title() -> std::string {}
auto WindowGLFW::SetTitle(std::string title) -> void {}
auto WindowGLFW::ToInternal(win::KeyType key) -> win::Key {}
auto WindowGLFW::ToExternal(win::Key key) -> win::KeyType {}
} // namespace glfw

} // namespace nj
