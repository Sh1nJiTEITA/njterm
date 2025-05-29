#include "njcon.h"
#include "njlog.h"

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
        } -- vk
    } -- END

)lua";

Config::Config() : state{stdConfigFile} {
    nj::log::Debug("Loading default config");
}
Config::Config(const std::string &config_data) : state{config_data} {
    nj::log::Debug("Loading not default config");
}
Config::Config(std::string &&config_data) noexcept
    : state{std::move(config_data)} {
    nj::log::Debug("Loading not default config");
}

} // namespace nj::con
