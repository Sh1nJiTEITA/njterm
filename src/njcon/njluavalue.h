#pragma once
#ifndef NJ_LUA_VALUE_H
#define NJ_LUA_VALUE_H

#include "njluautils.h"
#include <utility>
#include <vector>

namespace nj::lua {

class Value {
    using Pair = std::pair<Value, Value>;

  public:
    enum Type : int {
        Nil = 0,
        Boolean = 1,
        LightUserData = 2,
        Number = 3,
        String = 4,
        Table = 5,
        Function = 6,
        UserData = 7,
        Thread = 8,
        None = -1,
    };

    explicit Value(LuaStatePtrWeak &&ptr, int ref);
    ~Value();

    template <typename T> bool Is() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(rawState(), ref);
        bool result = false;
        if constexpr (meta::IsIntOnly<_T>::value) {
            result = lua_isinteger(rawState(), -1);
        } else if constexpr (meta::IsBool<_T>::value) {
            result = lua_isboolean(rawState(), -1);
        } else if constexpr (meta::IsReal<_T>::value) {
            result = lua_isnumber(rawState(), -1);
        } else if constexpr (meta::IsString<_T>::value) {
            result = lua_isstring(rawState(), -1);
        }
        return result;
    }

    template <Type t> bool Is() {
        const PushLuaValue p(rawState(), ref);
        bool result = false;
        if constexpr (t == Type::Nil) {
            result = lua_isnil(rawState(), -1);
        } else if constexpr (t == Type::Boolean) {
            result = lua_isboolean(rawState(), -1);
        } else if constexpr (t == Type::LightUserData) {
            result = lua_islightuserdata(rawState(), -1);
        } else if constexpr (t == Type::Number) {
            result = lua_isnumber(rawState(), -1);
        } else if constexpr (t == Type::String) {
            result = lua_isstring(rawState(), -1);
        } else if constexpr (t == Type::Table) {
            result = lua_istable(rawState(), -1);
        } else if constexpr (t == Type::Function) {
            result = lua_isfunction(rawState(), -1);
        } else if constexpr (t == Type::UserData) {
            result = lua_isuserdata(rawState(), -1);
        } else if constexpr (t == Type::Thread) {
            result = lua_isthread(rawState(), -1);
        } else if constexpr (t == Type::None) {
            result = lua_isnone(rawState(), -1);
        }
        return result;
    }

    template <typename T> T As() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(rawState(), ref);
        T result{};
        if constexpr (meta::IsIntOnly<_T>::value) {
            result = lua_tointeger(rawState(), -1);
        } else if constexpr (meta::IsBool<_T>::value) {
            result = lua_toboolean(rawState(), -1);
        } else if constexpr (meta::IsReal<_T>::value) {
            result = lua_tonumber(rawState(), -1);
        } else if constexpr (meta::IsString<_T>::value) {
            result = lua_tostring(rawState(), -1);
        }
        return result;
    }

    Type LuaType();
    const char *LuaTypeStr();

    Value Field(const char *name);
    std::vector<Pair> Items();
    int Length();

  protected:
    LuaState *rawState();

  private:
    LuaStatePtrWeak source;
    int ref;
};

} // namespace nj::lua

#endif
