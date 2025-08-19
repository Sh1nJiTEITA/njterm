#pragma once
#ifndef NJ_WIN_H
#define NJ_WIN_H

#include "njkey.h"
#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan_shared.hpp>

namespace nj {
namespace win {

// clang-format off
class Window {
  public:
    virtual ~Window();
    virtual auto Create() -> void = 0;
    virtual auto Extent() -> glm::vec2 = 0;
    virtual auto Title() -> std::string = 0;
    virtual auto SetTitle(std::string title) -> void = 0;
    virtual auto VulkanExtensions() const -> std::vector<std::string> = 0;
    virtual auto CreateSurface(vk::SharedInstance inst) -> vk::SharedSurfaceKHR = 0;
    virtual auto ShouldClose() -> bool = 0;
    virtual auto Update() -> void = 0;
    virtual auto WaitToRecreate() -> void = 0;
};
// clang-format on

class KeyControl {
  public:
    virtual auto ToInternal(KeyType key) -> Key = 0;
    virtual auto ToExternal(Key key) -> KeyType = 0;
};

auto CreateWindow(const glm::ivec2 &ext) -> std::unique_ptr<Window>;
auto CreateKeyControl() -> std::unique_ptr<KeyControl>;

//! FIXME: Supports for now only x11 & wayland
auto DisplayServer() -> int;
auto IsKeyPressed(const KeyControl &ctl, Key key) -> bool;

} // namespace win
} // namespace nj

#endif
