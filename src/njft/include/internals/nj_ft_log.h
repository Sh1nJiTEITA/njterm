#pragma once
#include <fmt/color.h>
#ifndef NJ_FT_LOG_H
#define NJ_FT_LOG_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "njlog.h"

namespace nj::log {
template <typename... Args>
inline void CheckCall(FT_Error error, fmt::format_string<Args...> str,
                      Args &&...args) {
    if (error) {
        log::Fatal("Freetype error occured");
        fmt::print(stderr, "Local msg: {}",
                   fmt::format(str, std::forward<Args>(args)...));
        fmt::print(stderr, "Freetype msg: {}", FT_Error_String(error));
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nj::log

#endif
