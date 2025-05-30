#include "njcon.h"
#include "njlog.h"
#include "njluavalue.h"
#include <memory>

namespace nj::con {
// DEFAULT CONFIG VALUES
const char *stdConfigFile = R"lua( 

    return { 
        vk = { 
            validation = { 
                enabled = true, 
                layers = { 
                    "VK_LAYER_KHRONOS_validation"
                }
            } -- validation
        }, -- vk
        test = { 
            field = 1488
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

auto ValidationLayersSet() -> std::set<std::string> {
    return Get("vk.validation.layers").As<std::set<std::string>>();
}

} // namespace nj::con
