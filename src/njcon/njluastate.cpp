#include "njluastate.h"
#include "njlog.h"
#include "njluaexc.h"
#include "njlualog.h"
#include "njluavalue.h"
#include <lua.h>
#include <string>

namespace nj::lua {

static void LuaStateDeleter(LuaState *state) {
    lua_close(state);
    nj::log::Debug("Deleting Lua state");
};

State::State(const char *ctx, bool loadstd)
    : state{LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter)} {
    nj::log::Debug("Creating new Lua state");
    luaL_openlibs(state.get());
    Exec(ctx);
}

State::State(const std::string &ctx, bool loadstd)
    : state{LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter)} {

    nj::log::Debug("Creating new Lua state");
    luaL_openlibs(state.get());
    Exec(ctx);
}

State::State(std::string &&con, bool loadstd)
    : state{LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter)} {

    nj::log::Debug("Creating new Lua state");
    luaL_openlibs(state.get());
    Exec(std::move(con));
}

Value State::ReturnTable() {
    if (lua_gettop(state.get()) == 0) {
        throw exc::NoReturnTable();
    }
    if (!lua_istable(state.get(), -1)) {
        const char *last_name = luaL_typename(state.get(), -1);
        nj::log::Error("Lua chunk did not return a table, got: {}", last_name);
        throw exc::NoReturnTable();
    }
    int ref = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    LuaStatePtrWeak source{state};
    return Value(std::move(source), ref);
}

Value State::Global(const char *name) {
    int res = lua_getglobal(state.get(), name);
    int ref = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    LuaStatePtrWeak source{state};
    return Value(std::move(source), ref);
}

void State::Exec(const std::string &ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (const&)");
}

void State::Exec(std::string &&ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (&&)");
}

void State::Exec(const char *ctx) {
    auto res = luaL_dostring(state.get(), ctx);
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (&&)");
}

} // namespace nj::lua
