#include "njwin.h"

namespace nj {
namespace win {

Window::~Window() {}
KeyControl::~KeyControl() {}

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
