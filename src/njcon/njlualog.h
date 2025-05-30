#pragma once
#ifndef NJ_LUA_LOG_H
#define NJ_LUA_LOG_H

#include "njlog.h"

#include <lua.hpp>

namespace nj::log {
template <typename... Args>
inline auto CheckLuaCall(int res, lua_State *state,
                         fmt::format_string<Args...> str, Args &&...args) {
    if (res != LUA_OK) {
        luaL_traceback(state, state, lua_tostring(state, -1), 1);
        const char *traceback = lua_tostring(state, -1);
        log::Fatal(str, std::forward<Args>(args)...);
        fmt::print(stderr, "Lua info: {}", traceback);
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nj::log

#endif
