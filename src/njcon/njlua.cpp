#include "njlua.h"
#include "njlog.h"
#include "njlualog.h"

#include <cerrno>
#include <lua.h>
#include <memory>
#include <string>
#include <vector>

namespace nj::lua {

Value::Value(LuaStatePtrWeak &&ptr, int ref)
    : source{std::move(ptr)}, ref{ref} {}

Value::~Value() { luaL_unref(rawState(), LUA_REGISTRYINDEX, ref); }

LuaState *Value::rawState() {
    if (auto lock = source.lock()) {
        return lock.get();
    } else {
        nj::log::Error("Cant lock lua-state for value");
        std::exit(EXIT_FAILURE);
    }
}

Value::Type Value::LuaType() {
    // clang-format off
    const PushLuaValue p(rawState(), ref);  
    int type = lua_type(rawState(), -1);
    switch (type) {
    case LUA_TNIL: return Type::Nil;
    case LUA_TBOOLEAN: return Type::Boolean;
    case LUA_TLIGHTUSERDATA: return Type::LightUserData;
    case LUA_TNUMBER: return Type::Number;
    case LUA_TSTRING: return Type::String;
    case LUA_TTABLE: return Type::Table;
    case LUA_TFUNCTION: return Type::Function;
    case LUA_TUSERDATA: return Type::UserData;
    case LUA_TTHREAD: return Type::Thread;
    case LUA_TNONE: return Type::None;
    default: return Type::None;
    }
    // clang-format on
}

const char *Value::LuaTypeStr() {
    LuaState *st = rawState();
    const Type t = LuaType();
    const PushLuaValue p(st, ref);
    return lua_typename(st, t);
}

Value Value::Field(const char *name) {
    if (!Is<Type::Table>()) {
        nj::log::FatalExit(
            "Cant get field from none table lua-object. Current Type={}",
            LuaTypeStr());
    }
    LuaState *st = rawState();
    const PushLuaValue p(st, this->ref);
    lua_getfield(st, -1, name);
    int ref = luaL_ref(st, LUA_REGISTRYINDEX);
    LuaStatePtrWeak source = this->source;
    return Value(std::move(source), ref);
}

std::vector<Value::Pair> Value::Items() {
    std::vector<Value::Pair> items;
    LuaState *st = rawState();
    const PushLuaValue p(st, this->ref);
    lua_pushnil(st);
    while (lua_next(st, -2) != 0) {

        lua_pushvalue(st, -2);
        int key = luaL_ref(st, LUA_REGISTRYINDEX);
        lua_pushvalue(st, -1);
        int val = luaL_ref(st, LUA_REGISTRYINDEX);
        items.emplace_back(Value(LuaStatePtrWeak(source), key),
                           Value(LuaStatePtrWeak(source), val));
        lua_pop(st, 1);
    }
    return items;
}

int Value::Length() {
    if (!Is<Type::Table>()) {
        nj::log::FatalExit("Cant get table length. Current lua object Type={}",
                           LuaTypeStr());
    }
    LuaState *st = rawState();
    const PushLuaValue p(st, this->ref);
    return lua_rawlen(st, -1);
}

PushLuaValue::PushLuaValue(LuaState *state, int ref) : state{state} {
    lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
}
PushLuaValue::~PushLuaValue() { lua_pop(state, 1); }

static void LuaStateDeleter(LuaState *state) {
    lua_close(state);
    nj::log::Debug("Deleting Lua state");
};

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

} // namespace nj::lua
