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

State::State(std::string_view ctx, bool loadstd)
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
    LuaStatePtrWeak source{state};
    return Value(std::move(source), tableRef);
}

Value State::Global(std::string_view name) {
    int res = lua_getglobal(state.get(), name.data());
    int ref = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    LuaStatePtrWeak source{state};
    return Value(std::move(source), ref);
}

void State::Exec(const std::string &ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (const&)");
    if (lua_gettop(state.get()) != 0 && lua_istable(state.get(), -1)) {
        tableRef = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    }
}

void State::Exec(std::string &&ctx) {
    auto res = luaL_dostring(state.get(), ctx.c_str());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (&&)");
    if (lua_gettop(state.get()) != 0 && lua_istable(state.get(), -1)) {
        tableRef = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    }
}

void State::Exec(std::string_view ctx) {
    auto res = luaL_dostring(state.get(), ctx.data());
    nj::log::CheckLuaCall(res, state.get(), "Cant execute lua-code (&&)");
    if (lua_gettop(state.get()) != 0 && lua_istable(state.get(), -1)) {
        tableRef = luaL_ref(state.get(), LUA_REGISTRYINDEX);
    }
}

void State::ResetExec(const std::string &con) {
    state.reset();
    state = LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter);
    luaL_openlibs(state.get());
    Exec(con);
}

void State::ResetExec(std::string &&con) {
    state.reset();
    state = LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter);
    luaL_openlibs(state.get());
    Exec(std::move(con));
}

void State::ResetExec(std::string_view con) {
    state.reset();
    state = LuaStatePtrShared(luaL_newstate(), &LuaStateDeleter);
    luaL_openlibs(state.get());
    Exec(con);
}

} // namespace nj::lua
