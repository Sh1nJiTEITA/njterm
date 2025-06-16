#pragma once
#ifndef NJ_LOG_H
#define NJ_LOG_H

#include <atomic>
#include <cerrno>
#include <chrono>
#include <exception>
#include <fmt/base.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <utility>

namespace nj {
namespace log {

using LevelType = uint8_t;
static std::atomic<LevelType> LevelBitSet = UINT8_MAX;
enum class Level : LevelType {
    Debug = 1 << 0,
    Info = 1 << 1,
    Warn = 1 << 2,
    Error = 1 << 3,
    Fatal = 1 << 4
};

inline void EnableLevel(LevelType level) { LevelBitSet.fetch_or(level); }
inline void DisableLevel(LevelType level) { LevelBitSet.fetch_and(~level); }

template <Level l> constexpr const char *LevelString() {
    // clang-format off
    if constexpr (l == Level::Debug) return " DEBUG ";
    else if constexpr (l == Level::Info) return " INFO ";
    else if constexpr (l == Level::Warn) return " WARN ";
    else if constexpr (l == Level::Error) return " ERROR ";
    else if constexpr (l == Level::Fatal) return " FATAL ";
    else return " UNKNOWN ";
    // clang-format on
}

// clang-format off
inline const fmt::text_style FmtStyleDebug = fmt::bg(fmt::color::orange) | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleInfo  = fmt::bg(fmt::color::green)  | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleWarn  = fmt::bg(fmt::color::yellow) | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleError = fmt::bg(fmt::color::red)    | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleFatal = fmt::bg(fmt::color::red)    | fmt::emphasis::bold;
inline const fmt::text_style FmtStyleTime =  fmt::emphasis::italic;
// clang-format on

template <Level l, typename... Args>
inline void Log(fmt::text_style style, fmt::format_string<Args...> str,
                Args &&...args) {
    const auto bitset = LevelBitSet.load();
    const bool is_level = bitset & static_cast<LevelType>(l);
    if (!is_level) {
        return;
    }
    using Clock = std::chrono::system_clock;
    using Seconds = std::chrono::seconds;
    const auto now = std::chrono::floor<Seconds>(Clock::now());
    fmt::print(stderr, style, LevelString<l>());
    fmt::print(stderr, FmtStyleTime, " [{:%H:%M:%S}] ", now);
    fmt::println(stderr, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void Debug(fmt::format_string<Args...> str, Args &&...args) {
    Log<Level::Debug>(FmtStyleDebug, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void Info(fmt::format_string<Args...> str, Args &&...args) {
    Log<Level::Info>(FmtStyleInfo, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void Warn(fmt::format_string<Args...> str, Args &&...args) {
    Log<Level::Warn>(FmtStyleWarn, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void Error(fmt::format_string<Args...> str, Args &&...args) {
    Log<Level::Error>(FmtStyleError, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void Fatal(fmt::format_string<Args...> str, Args &&...args) {
    Log<Level::Fatal>(FmtStyleFatal, str, std::forward<Args>(args)...);
}

template <typename... Args>
inline auto CheckUnixCall(int status, fmt::format_string<Args...> str,
                          Args &&...args) {
    if (status == -1) {
        log::Fatal(str, std::forward<Args>(args)...);
        fmt::print(stderr, "Error info: {}", std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }
}

template <typename... Args>
inline auto FatalExit(fmt::format_string<Args...> str, Args &&...args) {
    log::Fatal(str, std::forward<Args>(args)...);
    std::exit(EXIT_FAILURE);
}

template <typename... Args>
inline auto FatalThrow(std::exception &&exc, fmt::format_string<Args...> str,
                       Args &&...args) {
    log::Fatal(str, std::forward<Args>(args)...);
    throw std::forward<std::exception>(exc);
}

} // namespace log
} // namespace nj

#endif
