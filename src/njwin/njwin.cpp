#include "njwin.h"
#include <memory>

#if defined(NJ_USE_IMPL_GLFW)
#include "njglfw.h"
#endif

namespace nj {
namespace win {

Window::~Window() {}

auto CreateWindow() -> std::unique_ptr<Window> {
#if defined(NJ_USE_IMPL_GLFW)
    return std::make_unique<glfw::WindowGLFW>("njterm", glm::vec2{600, 600});
#elif
    static_assert(false, "No window impl is chosen");
#endif
}

auto CreateKeyControl() -> std::unique_ptr<KeyControl> {
#if defined(NJ_USE_IMPL_GLFW)
    return std::make_unique<glfw::KeyControlGLFW>();
#elif
    static_assert(false, "No window impl is chosen");
#endif
}

auto IsKeyPressed(const KeyControl &ctl, Key key) -> bool { return true; }
auto DisplayServer() -> int {
    if (std::getenv("DISPLAY"))
        return 0;
    if (std::getenv("WAYLAND_DISPLAY"))
        return 1;
    return -1;
}

} // namespace win

} // namespace nj
