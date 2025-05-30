#pragma once
#ifndef NJ_LUA_UTILS_H
#define NJ_LUA_UTILS_H

#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

class lua_State;

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

template <typename T> struct IsVector : std::false_type {};
template <typename T, typename Alloc>
struct IsVector<std::vector<T, Alloc>> : std::true_type {};
template <typename T> struct VectorElementType;
template <typename T, typename Alloc>
struct VectorElementType<std::vector<T, Alloc>> {
    using value_type = T;
};

template <typename T> struct IsSet : std::false_type {};
template <typename T, typename Compare, typename Alloc>
struct IsSet<std::set<T, Compare, Alloc>> : std::true_type {};
template <typename T> struct SetElementType;
template <typename T, typename Compare, typename Alloc>
struct SetElementType<std::set<T, Compare, Alloc>> {
    using value_type = T;
};

} // namespace meta

using LuaState = lua_State;
using LuaStatePtrShared = std::shared_ptr<LuaState>;
using LuaStatePtrWeak = std::weak_ptr<LuaState>;
using LuaRef = int;
using LuaRefPtrShared = std::shared_ptr<int>;

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
