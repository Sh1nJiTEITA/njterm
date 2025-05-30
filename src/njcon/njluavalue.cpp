#include "njluavalue.h"
#include "njlog.h"
#include "njluaexc.h"
#include "njluautils.h"
#include <iterator>
#include <lua.h>
#include <memory>
#include <optional>
#include <ranges>
#include <string_view>

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

LuaState *Value::RawState() {
    if (auto lock = source.lock()) {
        return lock.get();
    } else {
        nj::log::Error("Cant lock lua-state for value");
        std::exit(EXIT_FAILURE);
    }
}

Value::Type Value::LuaType() {
    // clang-format off
    const PushLuaValue p(RawState(), *ref);  
    int type = lua_type(RawState(), -1);
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

std::string_view Value::LuaTypeStr() {
    LuaState *st = RawState();
    const Type t = LuaType();
    const PushLuaValue p(st, *ref);
    return lua_typename(st, t);
}

Value Value::Field(std::string_view name) {
    auto field = FieldMaybe(name);
    if (!field.has_value()) {
        throw exc::NoFieldInTable();
    }
    return field.value();
}

Value Value::operator[](std::string_view name) { return Field(name); }

std::optional<Value> Value::FieldMaybe(std::string_view name) {
    if (!Is<Type::Table>()) {
        log::Error("Cant get field=\"{}\" from table. Not a table", name);
        return std::nullopt;
    }
    LuaState *st = RawState();
    PushLuaValue p(st, *this->ref);

    auto null_str = std::string(name);
    auto type = lua_getfield(st, -1, null_str.c_str());
    if (type == LUA_TNIL) {
        log::Error("Cant get field=\"{}\" from table. "
                   "Value does not exists",
                   name);
        p.kill();
        return std::nullopt;
    }
    LuaRef ref = luaL_ref(st, LUA_REGISTRYINDEX);
    LuaStatePtrWeak source = this->source;
    return Value(std::move(source), ref);
}

std::vector<std::string_view>
SplitWindowBySingleSeparator(std::string_view path, char separator) {
    std::vector<std::string_view> parts;
    const auto drop = [&separator](char c) { return c == separator; };
    const auto split = std::views::split(separator);
    const auto transform =
        std::views::transform([](auto &&rng) -> std::string_view {
            auto it = rng.begin();
            auto end = rng.end();
            if (it == end)
                return {};
            return std::string_view(&*rng.begin(), std::ranges::distance(rng));
        });
    for (auto part : path | split | transform) {
        parts.push_back(part);
    }
    return parts;
}

std::optional<Value> Value::PathMaybe(std::string_view name) {
    auto split = SplitWindowBySingleSeparator(name, '.');
    Value current = *this;
    for (auto part : split) {
        auto next = current.FieldMaybe(part);
        if (!next.has_value()) {
            return std::nullopt;
        }
        current = next.value();
    }
    return current;
}

std::vector<Value::Pair> Value::Pairs() {
    std::vector<Value::Pair> items;
    if (!Is<Type::Table>()) {
        nj::log::Error("Cant get table keys. Current value is not a table");
    }
    LuaState *st = RawState();
    PushLuaValue p(st, *this->ref);
    LuaStatePtrWeak source{this->source};
    lua_pushnil(st);
    while (lua_next(st, -2) != 0) {
        lua_pushvalue(st, -2);
        LuaRef key = luaL_ref(st, LUA_REGISTRYINDEX);
        LuaRef val = luaL_ref(st, LUA_REGISTRYINDEX);
        items.emplace_back(Value(source, key), Value(source, val));
    }
    return items;
}

std::vector<Value::IPair> Value::IPairs() {
    std::vector<Value::IPair> items;
    if (!Is<Type::Table>()) {
        nj::log::Error("Cant get table values. Current value is not a table");
        return items;
    }
    LuaState *st = RawState();
    PushLuaValue p(st, *this->ref);
    LuaStatePtrWeak source{this->source};
    int index = 1;
    while (true) {
        lua_rawgeti(st, -1, index);
        if (lua_isnil(st, -1)) {
            lua_pop(st, 1);
            break;
        }

        LuaRef val = luaL_ref(st, LUA_REGISTRYINDEX);
        items.emplace_back(index, Value(source, val));
        index++;
    }
    return items;
}

std::vector<Value> Value::Keys() {
    std::vector<Value> keys;
    if (!Is<Type::Table>()) {
        nj::log::Error("Cant get table keys. Current value is not a table");
    }
    LuaState *st = RawState();
    PushLuaValue p(st, *this->ref);
    lua_pushnil(st);

    while (lua_next(st, -2) != 0) {
        lua_pushvalue(st, -2);
        LuaRef ref = luaL_ref(st, LUA_REGISTRYINDEX);
        LuaStatePtrWeak source{this->source};
        keys.emplace_back(source, ref);
        lua_pop(st, 1);
    }
    return keys;
}

int Value::Length() {
    if (!Is<Type::Table>()) {
        nj::log::FatalExit("Cant get table length. Current lua object Type={}",
                           LuaTypeStr());
    }
    LuaState *st = RawState();
    const PushLuaValue p(st, *this->ref);
    return lua_rawlen(st, -1);
}
} // namespace nj::lua
