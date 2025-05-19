#pragma once

#include <cstring>
#ifndef NJ_LOG_H
#define NJ_LOG_H

#include <chrono>
#include <fmt/base.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include <utility>

namespace nj {
namespace log {

// clang-format off
inline const fmt::text_style FmtStyleDebug = fmt::bg(fmt::color::orange) | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleInfo  = fmt::bg(fmt::color::green)  | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleWarn  = fmt::bg(fmt::color::yellow) | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleError = fmt::bg(fmt::color::red)    | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleFatal = fmt::bg(fmt::color::red)    | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleTime =  fmt::emphasis::italic;
// clang-format on

template <typename... Args>
inline void Log(const char *level_name, fmt::text_style style, Args &&...args) {
    fmt::print(stderr, style, level_name);
    const auto time = fmt::localtime(std::time(nullptr));
    fmt::print(stderr, FmtStyleTime, " [{:%H:%M:%S}] ", time);
    fmt::println(stderr, std::forward<Args>(args)...);
}

template <typename... Args> inline void Debug(Args &&...args) {
    Log(" DEBUG ", FmtStyleDebug, std::forward<Args>(args)...);
}

template <typename... Args> inline void Info(Args &&...args) {
    Log(" INFO ", FmtStyleInfo, std::forward<Args>(args)...);
}

template <typename... Args> inline void Warn(Args &&...args) {
    Log(" WARN ", FmtStyleWarn, std::forward<Args>(args)...);
}

template <typename... Args> inline void Error(Args &&...args) {
    Log(" ERROR ", FmtStyleError, std::forward<Args>(args)...);
}

template <typename... Args> inline void Fatal(Args &&...args) {
    Log(" FATAL ", FmtStyleFatal, std::forward<Args>(args)...);
}

template <typename... Args> inline auto CheckCall(int status, Args &&...args) {
    if (status == -1) {
        log::Fatal(std::forward<Args>(args)...);
        fmt::print("Error info: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }
}

} // namespace log
} // namespace nj

#endif
