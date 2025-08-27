#pragma once
#ifndef NJ_CON_H
#define NJ_CON_H

#include "njluastate.h"
#include <filesystem>
#include <string>

namespace nj::con {

class Config {
  public:
    Config();
    Config(const std::string &config_data);
    Config(std::string &&config_data) noexcept;

    //! Loads config from different sources. If config was loaded before ->
    //! totally reloads it with closing existing lua-state. All values, which
    //! were created from previous state will become unrecheable (weak ptr will
    //! dongle)
    //! @param config_data Source of lua code
    auto Load(const std::string &config_data) -> void;
    auto Load(std::string &&config_data) noexcept -> void;
    auto Load(const std::filesystem::path &path) -> void;

  public: // export
    auto Get(std::string_view path) -> nj::lua::Value;

  private:
    lua::State state;
    lua::State defaultState;
};

//! See Config::Load comments. Does same logic but for config instance
//! (singleton)
//! @param config_data Source of lua code
auto Load(const std::string &config_data) -> void;
auto Load(std::string &&config_data) noexcept -> void;
auto Load(const std::filesystem::path &path) -> void;

auto Get(std::string_view path) -> lua::Value;
auto ValidationEnabled() -> bool;
auto ValidationLayers() -> std::vector<std::string>;
auto ValidationExtensions() -> std::vector<std::string>;
auto ValidationFeatures() -> std::vector<int>;
auto Buffering() -> uint32_t;
auto Frames() -> uint32_t;
auto FrameObjectsMode() -> uint32_t;

} // namespace nj::con

#endif
