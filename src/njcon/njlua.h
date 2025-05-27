#pragma once
#include <vector>
#ifndef NJ_LUA_H
#define NJ_LUA_H

#include <lua.hpp>

#include <memory>

namespace nj::lua {

namespace meta {
template <typename... Types> using And = std::conjunction<Types...>;
template <typename... Types> using Or = std::disjunction<Types...>;
template <typename T> using Not = std::negation<T>;

template <typename T> using IsBool = std::is_same<T, bool>;
template <typename T> using IsInt = std::is_integral<T>;
template <typename T> using IsReal = std::is_arithmetic<T>;
template <typename T> using IsIntOnly = And<IsInt<T>, Not<IsBool<T>>>;
template <typename T> using IsString = std::is_same<T, std::string>;

} // namespace meta

using LuaState = lua_State;
using LuaStatePtrShared = std::shared_ptr<LuaState>;
using LuaStatePtrWeak = std::weak_ptr<LuaState>;

class PushLuaValue {
  public:
    PushLuaValue(LuaState *state, int ref);
    ~PushLuaValue();

  private:
    LuaState *state;
};

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

class State {
  public:
    State(const std::string &con, bool loadstd = true);
    State(std::string &&con, bool loadstd = true);

    Value Global(const char *name);

    void Exec(const std::string &con);
    void Exec(std::string &&con);

  private:
    LuaStatePtrShared state;
};

} // namespace nj::lua

#endif
