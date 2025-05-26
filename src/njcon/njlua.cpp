#include "njlog.h"
#include "njlualog.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <njlua.h>
#include <string>

namespace nj::lua {

static void LuaStateDeleter(LuaState *state) {
    lua_close(state);
    nj::log::Debug("Deleting Lua state");
};

State::State(const std::string &ctx, bool loadstd)
    : state{LuaStatePtrType(luaL_newstate(), &LuaStateDeleter)} {

    nj::log::Debug("Creating new Lua state");
    luaL_openlibs(state.get());
    Exec(ctx);
}

State::State(std::string &&con, bool loadstd)
    : state{LuaStatePtrType(luaL_newstate(), &LuaStateDeleter)} {

    nj::log::Debug("Creating new Lua state");
    luaL_openlibs(state.get());
    Exec(std::move(con));
}

void State::Exec(const std::string &ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (const&)");
}

void State::Exec(std::string &&ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (&&)");
}

} // namespace nj::lua
