#include "njglfw.h"
#include "njlog.h"
#include "njwin.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace nj {
namespace glfw {

WindowGLFW::WindowGLFW(std::string title, glm::vec2 init_ext)
    : title{title}, extent{init_ext} {
    glfwInit();
    const auto display_server = win::DisplayServer();
    log::CheckUnixCall(display_server,
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

    nj::log::CheckUnixCall(glfwWindowHandle == nullptr ? 1 : 0,
                           "Cant create GLFW window");
    // glfwSetFramebufferSizeCallback(glfwWindow, GLFWframebuffersizefun
    // callback)
}

auto WindowGLFW::Extent() -> glm::vec2 {
    glm::ivec2 ext;
    glfwGetFramebufferSize(glfwWindowHandle, &ext.x, &ext.y);
    return ext;
}

auto WindowGLFW::Title() -> std::string {
    return glfwGetWindowTitle(glfwWindowHandle);
}

auto WindowGLFW::SetTitle(std::string title) -> void {
    glfwSetWindowTitle(glfwWindowHandle, title.c_str());
}

auto WindowGLFW::VulkanExtensions() const -> std::vector<std::string> {
    uint32_t count = 0;
    const char **ext;
    ext = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> extensions(ext, ext + count);
    std::vector<std::string> res{extensions.begin(), extensions.end()};
    return res;
}

auto WindowGLFW::CreateSurface(vk::SharedInstance inst)
    -> vk::SharedSurfaceKHR {
    VkSurfaceKHR surface_;
    glfwCreateWindowSurface(*inst, glfwWindowHandle, nullptr, &surface_);
    return vk::SharedSurfaceKHR(surface_, inst);
}

auto KeyControlGLFW::ToInternal(win::KeyType key) -> win::Key {
    // clang-format off
    switch (key) { 
	case GLFW_KEY_CAPS_LOCK: return win::Key::CapsLock;
	case GLFW_KEY_SPACE: return win::Key::Space;
	case GLFW_KEY_TAB: return win::Key::Tab; 
	case GLFW_KEY_ENTER: return win::Key::Enter; 
	case GLFW_KEY_ESCAPE: return win::Key::Escape; 
	case GLFW_KEY_BACKSPACE: return win::Key::Backspace;
	case GLFW_KEY_SCROLL_LOCK: return win::Key::ScrollLock;
	case GLFW_KEY_DELETE: return win::Key::Delete;
	case GLFW_KEY_INSERT: return win::Key::Insert;
	case GLFW_KEY_HOME: return win::Key::Home;
	case GLFW_KEY_END: return win::Key::End;
	case GLFW_KEY_PAGE_UP: return win::Key::PgUp;
	case GLFW_KEY_PAGE_DOWN: return win::Key::PgDn;
	case GLFW_KEY_UP: return win::Key::Up;
	case GLFW_KEY_DOWN: return win::Key::Down;
	case GLFW_KEY_LEFT: return win::Key::Left;
	case GLFW_KEY_RIGHT: return win::Key::Right;
	case GLFW_KEY_F1: return win::Key::F1;
	case GLFW_KEY_F2: return win::Key::F2;
	case GLFW_KEY_F3: return win::Key::F3;
	case GLFW_KEY_F4: return win::Key::F4;
	case GLFW_KEY_F5: return win::Key::F5;
	case GLFW_KEY_F6: return win::Key::F6;
	case GLFW_KEY_F7: return win::Key::F7;
	case GLFW_KEY_F8: return win::Key::F8;
	case GLFW_KEY_F9: return win::Key::F9;
	case GLFW_KEY_F10: return win::Key::F10;
	case GLFW_KEY_F11: return win::Key::F11;
	case GLFW_KEY_F13: return win::Key::F13;
	case GLFW_KEY_F14: return win::Key::F14;
	case GLFW_KEY_F15: return win::Key::F15;
	case GLFW_KEY_F16: return win::Key::F16;
	case GLFW_KEY_F17: return win::Key::F17;
	case GLFW_KEY_F18: return win::Key::F18;
	case GLFW_KEY_F19: return win::Key::F19;
	case GLFW_KEY_F20: return win::Key::F20;
	case GLFW_KEY_F21: return win::Key::F21;
	case GLFW_KEY_F22: return win::Key::F22;
	case GLFW_KEY_F23: return win::Key::F23;
	case GLFW_KEY_F24: return win::Key::F24;
	case GLFW_KEY_LEFT_SUPER: return win::Key::LeftWin;
	case GLFW_KEY_RIGHT_SUPER: return win::Key::RightWin;
	case GLFW_KEY_LEFT_CONTROL: return win::Key::LeftCtrl;
	case GLFW_KEY_RIGHT_CONTROL: return win::Key::RightCtrl;
	case GLFW_KEY_LEFT_SHIFT: return win::Key::LeftShift;
	case GLFW_KEY_RIGHT_SHIFT: return win::Key::RightShift;
	case GLFW_KEY_RIGHT_ALT: return win::Key::RightAlt;
	case GLFW_KEY_MENU: return win::Key::Apps; // what is it? :)
	case GLFW_KEY_PRINT_SCREEN: return win::Key::PrintScreen;
	case GLFW_KEY_PAUSE: return win::Key::Pause;
	case GLFW_MOUSE_BUTTON_LEFT: return win::Key::MouseLeft;
	case GLFW_MOUSE_BUTTON_RIGHT: return win::Key::MouseRight;
	case GLFW_MOUSE_BUTTON_MIDDLE: return win::Key::MouseMiddle;
	case GLFW_KEY_Q: return win::Key::Q;
	case GLFW_KEY_W: return win::Key::W;
	case GLFW_KEY_E: return win::Key::E;
	case GLFW_KEY_R: return win::Key::R;
	case GLFW_KEY_T: return win::Key::T;
	case GLFW_KEY_Y: return win::Key::Y;
	case GLFW_KEY_U: return win::Key::U;
	case GLFW_KEY_I: return win::Key::I;
	case GLFW_KEY_O: return win::Key::O;
	case GLFW_KEY_P: return win::Key::P;
	case GLFW_KEY_A: return win::Key::A;
	case GLFW_KEY_S: return win::Key::S;
	case GLFW_KEY_D: return win::Key::D;
	case GLFW_KEY_F: return win::Key::F;
	case GLFW_KEY_G: return win::Key::G;
	case GLFW_KEY_H: return win::Key::H;
	case GLFW_KEY_J: return win::Key::J;
	case GLFW_KEY_K: return win::Key::K;
	case GLFW_KEY_L: return win::Key::L;
	case GLFW_KEY_Z: return win::Key::Z;
	case GLFW_KEY_X: return win::Key::X;
	case GLFW_KEY_C: return win::Key::C;
	case GLFW_KEY_V: return win::Key::V;
	case GLFW_KEY_B: return win::Key::B;
	case GLFW_KEY_N: return win::Key::N;
	case GLFW_KEY_M: return win::Key::M;
	case GLFW_KEY_SLASH: return win::Key::Slash;
	case GLFW_KEY_BACKSLASH: return win::Key::BackSlash;
	case GLFW_KEY_SEMICOLON: return win::Key::SemiColon;
	case GLFW_KEY_APOSTROPHE: return win::Key::Apostrophe;
        case GLFW_KEY_PERIOD: return win::Key::Period;
        case GLFW_KEY_COMMA: return win::Key::Comma;
	case GLFW_KEY_LEFT_BRACKET: return win::Key::LeftBrace; // <
        case GLFW_KEY_RIGHT_BRACKET: return win::Key::RightBrace; // <
	default: return win::Key::Underfined;
    }
}

auto KeyControlGLFW::ToExternal(win::Key key) -> win::KeyType {
    // clang-format off
    switch (key) { 
	case win::Key::CapsLock: return GLFW_KEY_CAPS_LOCK;
	case win::Key::Space: return GLFW_KEY_SPACE;
	case win::Key::Tab: return GLFW_KEY_TAB; 
	case win::Key::Enter: return GLFW_KEY_ENTER; 
	case win::Key::Escape: return GLFW_KEY_ESCAPE; 
	case win::Key::Backspace: return GLFW_KEY_BACKSPACE;
	case win::Key::ScrollLock: return GLFW_KEY_SCROLL_LOCK;
	case win::Key::Delete: return GLFW_KEY_DELETE;
	case win::Key::Insert: return GLFW_KEY_INSERT;
	case win::Key::Home: return GLFW_KEY_HOME;
	case win::Key::End: return GLFW_KEY_END;
	case win::Key::PgUp: return GLFW_KEY_PAGE_UP;
	case win::Key::PgDn: return GLFW_KEY_PAGE_DOWN;
	case win::Key::Up: return GLFW_KEY_UP;
	case win::Key::Down: return GLFW_KEY_DOWN;
	case win::Key::Left: return GLFW_KEY_LEFT;
	case win::Key::Right: return GLFW_KEY_RIGHT;
	case win::Key::F1: return GLFW_KEY_F1;
	case win::Key::F2: return GLFW_KEY_F2;
	case win::Key::F3: return GLFW_KEY_F3;
	case win::Key::F4: return GLFW_KEY_F4;
	case win::Key::F5: return GLFW_KEY_F5;
	case win::Key::F6: return GLFW_KEY_F6;
	case win::Key::F7: return GLFW_KEY_F7;
	case win::Key::F8: return GLFW_KEY_F8;
	case win::Key::F9: return GLFW_KEY_F9;
	case win::Key::F10: return GLFW_KEY_F10;
	case win::Key::F11: return GLFW_KEY_F11;
	case win::Key::F13: return GLFW_KEY_F13;
	case win::Key::F14: return GLFW_KEY_F14;
	case win::Key::F15: return GLFW_KEY_F15;
	case win::Key::F16: return GLFW_KEY_F16;
	case win::Key::F17: return GLFW_KEY_F17;
	case win::Key::F18: return GLFW_KEY_F18;
	case win::Key::F19: return GLFW_KEY_F19;
	case win::Key::F20: return GLFW_KEY_F20;
	case win::Key::F21: return GLFW_KEY_F21;
	case win::Key::F22: return GLFW_KEY_F22;
	case win::Key::F23: return GLFW_KEY_F23;
	case win::Key::F24: return GLFW_KEY_F24;
	case win::Key::LeftWin: return GLFW_KEY_LEFT_SUPER;
	case win::Key::RightWin: return GLFW_KEY_RIGHT_SUPER;
	case win::Key::Ctrl: return GLFW_KEY_LEFT_CONTROL;
	case win::Key::Alt: return GLFW_KEY_LEFT_ALT;
	case win::Key::Shift: return GLFW_KEY_LEFT_SHIFT;
	case win::Key::LeftCtrl: return GLFW_KEY_LEFT_CONTROL;
	case win::Key::RightCtrl: return GLFW_KEY_RIGHT_CONTROL;
	case win::Key::LeftShift: return GLFW_KEY_LEFT_SHIFT;
	case win::Key::RightShift: return GLFW_KEY_RIGHT_SHIFT;
	case win::Key::LeftAlt: return GLFW_KEY_LEFT_SHIFT;
	case win::Key::RightAlt: return GLFW_KEY_RIGHT_ALT;
	case win::Key::Apps: return GLFW_KEY_MENU; // what is it? :)
	case win::Key::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
	case win::Key::Pause: return GLFW_KEY_PAUSE;
	case win::Key::Help: return GLFW_KEY_UNKNOWN;
	case win::Key::Sleep: return GLFW_KEY_UNKNOWN;
	case win::Key::MouseLeft: return GLFW_MOUSE_BUTTON_LEFT;
	case win::Key::MouseRight: return GLFW_MOUSE_BUTTON_RIGHT;
	case win::Key::MouseMiddle: return GLFW_MOUSE_BUTTON_MIDDLE;
	case win::Key::Q: return GLFW_KEY_Q;
	case win::Key::W: return GLFW_KEY_W;
	case win::Key::E: return GLFW_KEY_E;
	case win::Key::R: return GLFW_KEY_R;
	case win::Key::T: return GLFW_KEY_T;
	case win::Key::Y: return GLFW_KEY_Y;
	case win::Key::U: return GLFW_KEY_U;
	case win::Key::I: return GLFW_KEY_I;
	case win::Key::O: return GLFW_KEY_O;
	case win::Key::P: return GLFW_KEY_P;
	case win::Key::A: return GLFW_KEY_A;
	case win::Key::S: return GLFW_KEY_S;
	case win::Key::D: return GLFW_KEY_D;
	case win::Key::F: return GLFW_KEY_F;
	case win::Key::G: return GLFW_KEY_G;
	case win::Key::H: return GLFW_KEY_H;
	case win::Key::J: return GLFW_KEY_J;
	case win::Key::K: return GLFW_KEY_K;
	case win::Key::L: return GLFW_KEY_L;
	case win::Key::Z: return GLFW_KEY_Z;
	case win::Key::X: return GLFW_KEY_X;
	case win::Key::C: return GLFW_KEY_C;
	case win::Key::V: return GLFW_KEY_V;
	case win::Key::B: return GLFW_KEY_B;
	case win::Key::N: return GLFW_KEY_N;
	case win::Key::M: return GLFW_KEY_M;
	case win::Key::Slash: return GLFW_KEY_SLASH;
	case win::Key::BackSlash: return GLFW_KEY_BACKSLASH;
	case win::Key::SemiColon: return GLFW_KEY_SEMICOLON;
	case win::Key::Apostrophe: return GLFW_KEY_APOSTROPHE;
        case win::Key::Period: return GLFW_KEY_PERIOD;
        case win::Key::Comma: return GLFW_KEY_COMMA;
	case win::Key::LeftBrace: return GLFW_KEY_LEFT_BRACKET; // <
        case win::Key::RightBrace: return GLFW_KEY_RIGHT_BRACKET; // <
        case win::Key::Underfined: return GLFW_KEY_UNKNOWN; // <
	default: return GLFW_KEY_UNKNOWN;
    }
}


} // namespace glfw

} // namespace nj
