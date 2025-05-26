#pragma once
#ifndef NJ_GLFW_H
#define NJ_GLFW_H

#include "njwin.h"
#include <GLFW/glfw3.h>

namespace nj {
namespace glfw {

class WindowGLFW : public win::Window {
  public:
    WindowGLFW(std::string title, glm::vec2 init_ext);
    ~WindowGLFW();
    virtual auto Create() -> void override;
    virtual auto Extent() -> glm::vec2 override;
    virtual auto Title() -> std::string override;
    virtual auto SetTitle(std::string title) -> void override;

  private:
    GLFWwindow *glfwWindowHandle;
    std::string title;
    glm::vec2 extent;
};

class KeyControlGLFW : public win::KeyControl {
    virtual auto ToInternal(win::KeyType key) -> win::Key override;
    virtual auto ToExternal(win::Key key) -> win::KeyType override;
};

} // namespace glfw
} // namespace nj

#endif
