
#pragma once
#ifndef NJ_LOG_SCOPE_H
#define NJ_LOG_SCOPE_H

#include "njlog.h"
#include <atomic>
#include <fmt/color.h>

namespace nj {
namespace log {

inline std::atomic<uint8_t> CurrentScopeLevel = 0;

// clang-format off
constexpr inline const char* GetArrow(uint8_t level) {
    switch (level) {
        case 0: return "";
        case 1: return "=> ";
        case 2: return "   => ";
        case 3: return "      => ";
        case 4: return "         => ";
        case 5: return "            => ";
        case 6: return "               => ";
        case 7: return "                  => ";
        case 8: return "                     => ";
        case 9: return "                        => ";
        case 10: return "                          => ";
        default: return "";
    }
}


//! @class ScopeLog provides way of logging during going in/out of scope. 
//! Can be usefull to check where something has broken without using debugger.
template <Level l> 
struct ScopeLog {
    const std::string msg;

    constexpr void Log(const char* postfix) {
        auto msg = fmt::format("{}{}", this->msg, postfix);
        if constexpr (l == Level::Debug) Debug("{}", msg);
        else if constexpr (l == Level::Info) Info("{}", msg);
        else if constexpr (l == Level::Warn) Warn("{}", msg);
        else if constexpr (l == Level::Error) Error("{}", msg);
        else if constexpr (l == Level::Fatal) Fatal("{}", msg);
    }

    template< typename ...Args >
    constexpr ScopeLog(fmt::format_string<Args...> str, Args&&... args) 
        : msg{ fmt::format(str, std::forward<Args>(args)...) }
    { 
        Log("..."); 
    }

    constexpr ~ScopeLog() { Log("... END"); }
};

template <typename... Args> 
constexpr inline auto DebugScope(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLog<Level::Debug>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto InfoScope(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLog<Level::Info>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto WarnScope(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLog<Level::Warn>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto ErrorScope(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLog<Level::Warn>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto FatalScope(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLog<Level::Fatal>(str, std::forward<Args>(args)...);
}

#define _NJ_LOG_CONCAT_IMPL(a, b) a##b
#define _NJ_LOG_CONCAT(a, b) _NJ_LOG_CONCAT_IMPL(a, b)

#define DEBUG_SCOPE(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::DebugScope(str, ##__VA_ARGS__)

#define INFO_SCOPE(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::InfoScope(str ##__VA_ARGS__)

#define WARN_SCOPE(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::WarnScope(str ##__VA_ARGS__)

#define ERROR_SCOPE(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::ErrorScope(str ##__VA_ARGS__)

#define FATAL_SCOPE(str, ...)\
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::FatalScope(str ##__VA_ARGS__)


//                            Arrowed version
// ****************************************************************************


inline const fmt::text_style FmtStyleArrow = fmt::fg(fmt::rgb(60,60,60));

template <Level l, typename... Args>
constexpr inline void
LogA(uint8_t level, fmt::text_style style, fmt::format_string<Args...> str, Args&&... args) {
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
    fmt::print(stderr, FmtStyleArrow, "{}", GetArrow(level));
    fmt::println(stderr, str, std::forward<Args>(args)...);
}

namespace detail {
    template <typename... Args>
    constexpr inline void DebugArrowed(uint8_t level, fmt::format_string<Args...> str, Args&&... args) {
        LogA<Level::Debug>(level, FmtStyleDebug, str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr inline void InfoArrowed(uint8_t level, fmt::format_string<Args...> str, Args&&... args) {
        LogA<Level::Info>(level, FmtStyleInfo, str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr inline void WarnArrowed(uint8_t level, fmt::format_string<Args...> str, Args&&... args) {
        LogA<Level::Warn>(level, FmtStyleWarn, str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr inline void ErrorArrowed(uint8_t level, fmt::format_string<Args...> str, Args&&... args) {
        LogA<Level::Error>(level, FmtStyleError, str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr inline void FatalArrowed(uint8_t level, fmt::format_string<Args...> str, Args&&... args) {
        LogA<Level::Fatal>(level, FmtStyleFatal, str, std::forward<Args>(args)...);
    }
}

template <typename... Args>
constexpr inline void DebugA(fmt::format_string<Args...> str, Args&&... args) {
    detail::DebugArrowed(CurrentScopeLevel.load(), str, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr inline void InfoA(fmt::format_string<Args...> str, Args&&... args) {
    detail::InfoArrowed(CurrentScopeLevel.load(), str, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr inline void WarnA(fmt::format_string<Args...> str, Args&&... args) {
    detail::WarnArrowed(CurrentScopeLevel.load(), str, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr inline void ErrorA(fmt::format_string<Args...> str, Args&&... args) {
    detail::ErrorArrowed(CurrentScopeLevel.load(), str, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr inline void FatalA(fmt::format_string<Args...> str, Args&&... args) {
    detail::FatalArrowed(CurrentScopeLevel.load(), str, std::forward<Args>(args)...);
}

template <Level l> 
struct ScopeLogArrowed {
    const std::string msg;
    const uint8_t level;

    constexpr void Log(const char* postfix) {
        auto msg = fmt::format("{}{}", this->msg, postfix);
        if constexpr (l == Level::Debug) detail::DebugArrowed(level, "{}", msg);
        else if constexpr (l == Level::Info) detail::InfoArrowed(level, "{}", msg);
        else if constexpr (l == Level::Warn) detail::WarnArrowed(level, "{}", msg);
        else if constexpr (l == Level::Error) detail::ErrorArrowed(level, "{}", msg);
        else if constexpr (l == Level::Fatal) detail::FatalArrowed(level, "{}", msg);
    }

    template< typename ...Args >
    constexpr ScopeLogArrowed(fmt::format_string<Args...> str, Args&&... args) 
        : msg{ fmt::format(str, std::forward<Args>(args)...) }
        , level{ CurrentScopeLevel.load(std::memory_order::relaxed) } 
    { 
        Log("..."); 
        CurrentScopeLevel.fetch_add(1);
    }

    constexpr ~ScopeLogArrowed() { 
        Log("... END"); 
        CurrentScopeLevel.fetch_add(-1);
    }
};

template <typename... Args> 
constexpr inline auto DebugScopeA(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLogArrowed<Level::Debug>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto InfoScopeA(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLogArrowed<Level::Info>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto WarnScopeA(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLogArrowed<Level::Warn>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto ErrorScopeA(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLogArrowed<Level::Warn>(str, std::forward<Args>(args)...);
}

template <typename... Args> 
constexpr inline auto FatalScopeA(fmt::format_string<Args...> str, Args&&... args) {
    return ScopeLogArrowed<Level::Fatal>(str, std::forward<Args>(args)...);
}

#define DEBUG_SCOPE_A(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::DebugScopeA(str, ##__VA_ARGS__)

#define INFO_SCOPE_A(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::InfoScopeA(str ##__VA_ARGS__)

#define WARN_SCOPE_A(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::WarnScopeA(str ##__VA_ARGS__)

#define ERROR_SCOPE_A(str, ...) \
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::ErrorScopeA(str ##__VA_ARGS__)

#define FATAL_SCOPE_A(str, ...)\
    auto _NJ_LOG_CONCAT(scope_handle_, __LINE__) = nj::log::FatalScopeA(str ##__VA_ARGS__)





} // namespace log
} // namespace nj

#endif
