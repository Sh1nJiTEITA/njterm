#include "njcon.h"
#include "njlog.h"
#include "njluavalue.h"
#include <memory>
#include <string>

namespace nj::con {
// DEFAULT CONFIG VALUES
const char *stdConfigFile = R"lua( 
    return { 
        vk = { 
            validation = { 
                enabled = true, 
                layers = { 
                    "VK_LAYER_KHRONOS_validation"
                },
                extensions = { 
                    "VK_EXT_debug_utils",
                    "VK_EXT_validation_features"
                },
                features = {
                    3 -- VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT  
                },
            }, -- validation
            buffering = 2,
            frames = 2,
            -- TODO: Does not work for now...

            ---@type "per_frame" | "per_image"
            frame_objects_mode = "per_frame"
        }, -- vk
        test = { 
            field = 1488,
        }
    } -- END

)lua";

Config::Config()
    : state{std::string_view(stdConfigFile)},
      defaultState{std::string_view(stdConfigFile)} {
    nj::log::Debug("Loading default config");
}
Config::Config(const std::string &config_data)
    : state{config_data}, defaultState{std::string_view(stdConfigFile)} {
    nj::log::Debug("Loading not default config");
}
Config::Config(std::string &&config_data) noexcept
    : state{std::move(config_data)},
      defaultState{std::string_view(stdConfigFile)} {
    nj::log::Debug("Loading not default config");
}

auto Config::Load(const std::string &config_data) -> void {
    state.ResetExec(config_data);
}
auto Config::Load(std::string &&config_data) noexcept -> void {
    state.ResetExec(config_data);
}
auto Config::Load(const std::filesystem::path &path) -> void {
    // TODO: IMPLEMENT
}
auto Config::Get(std::string_view path) -> nj::lua::Value {
    auto table = state.ReturnTable();
    auto val = table.PathMaybe(path);
    if (!val.has_value()) {
        auto std_table = defaultState.ReturnTable();
        auto std_val = std_table.PathMaybe(path);
        if (!std_val.has_value()) {
            log::FatalExit("Internal. Cant get standart "
                           "config value with path: {}",
                           path);
        }
        return std_val.value();
    }
    return val.value();
}

static Config &ConfigInstance() {
    static Config conf{};
    return conf;
}

auto Load(const std::string &config_data) -> void {
    auto &self = ConfigInstance();
    self.Load(config_data);
}
auto Load(std::string &&config_data) noexcept -> void {
    auto &self = ConfigInstance();
    self.Load(config_data);
}
auto Load(const std::filesystem::path &path) -> void {
    // IMPLEMENT
}

auto Get(std::string_view path) -> lua::Value {
    auto &self = ConfigInstance();
    return self.Get(path);
}

auto ValidationEnabled() -> bool {
    return Get("vk.validation.enabled").As<bool>();
}

auto ValidationLayers() -> std::vector<std::string> {
    return Get("vk.validation.layers").As<std::vector<std::string>>();
}

auto ValidationExtensions() -> std::vector<std::string> {
    return Get("vk.validation.extensions").As<std::vector<std::string>>();
}

auto ValidationFeatures() -> std::vector<int> {
    return Get("vk.validation.features").As<std::vector<int>>();
}

auto Buffering() -> uint32_t { return Get("vk.buffering").As<uint32_t>(); }

auto Frames() -> uint32_t { return Get("vk.frames").As<uint32_t>(); }

auto FrameObjectsMode() -> uint32_t {
    auto v = Get("vk.frame_objects_mode").As<std::string>();
    if (v == "per_frame") {
        return 0;
    } else if (v == "per_image") {
        return 1;
    } else {
        return 1;
    }
}

} // namespace nj::con
