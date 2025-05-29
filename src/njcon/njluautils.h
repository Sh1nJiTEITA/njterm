#pragma once
#ifndef NJ_LUA_UTILS_H
#define NJ_LUA_UTILS_H

#include <lua.hpp>
#include <memory>
#include <string>
#include <type_traits>

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
    void kill();

  private:
    bool isKilled;
    LuaState *state;
};

} // namespace nj::lua
#endif
