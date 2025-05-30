#include "njluavalue.h"
#include "njlog.h"
#include "njlualog.h"
#include "njluautils.h"
#include <memory>

namespace nj::lua {

LuaRefPtrShared MakeLuaValue(LuaStatePtrWeak state, LuaRef ref) {
    return LuaRefPtrShared(new int(ref), [state](LuaRef *refToDel) {
        auto lock = state.lock();
        if (lock && refToDel) {
            luaL_unref(lock.get(), LUA_REGISTRYINDEX, *refToDel);
        }
    });
}

Value::Value(LuaStatePtrWeak &&ptr, LuaRef r) noexcept
    : ref(MakeLuaValue(ptr, r)), source{std::move(ptr)} {}

Value::Value(const LuaStatePtrWeak &ptr, LuaRef r)
    : ref(MakeLuaValue(ptr, r)), source{ptr} {}

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
    const PushLuaValue p(rawState(), *ref);  
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
    const PushLuaValue p(st, *ref);
    return lua_typename(st, t);
}

Value Value::Field(const char *name) {
    if (!Is<Type::Table>()) {
        nj::log::FatalExit(
            "Cant get field from none table lua-object. Current Type={}",
            LuaTypeStr());
    }
    LuaState *st = rawState();
    const PushLuaValue p(st, *ref);
    lua_getfield(st, -1, name);
    int ref = luaL_ref(st, LUA_REGISTRYINDEX);
    LuaStatePtrWeak source = this->source;
    return Value(std::move(source), ref);
}

std::optional<Value> Value::FieldMaybe(const char *name) {
    if (!Is<Type::Table>()) {
        return {};
    }
    LuaState *st = rawState();
    LuaRef ref = LUA_NOREF;
    const PushLuaValue p(st, *this->ref);
    lua_getfield(st, -1, name);
    if (lua_isnil(st, -1)) {
        lua_pop(st, 1);
        return {};
    }
    ref = luaL_ref(st, LUA_REGISTRYINDEX);
    LuaStatePtrWeak source = this->source;
    return Value(std::move(source), ref);
}

std::vector<Value::Pair> Value::Items() {
    std::vector<Value::Pair> items;
    LuaState *st = rawState();
    const PushLuaValue p(st, *this->ref);
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
    const PushLuaValue p(st, *this->ref);
    return lua_rawlen(st, -1);
}
} // namespace nj::lua
