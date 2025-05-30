#pragma once
#ifndef NJ_LUA_VALUE_H
#define NJ_LUA_VALUE_H

#include "njluautils.h"
#include <lua.hpp>
#include <optional>
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

    Value() = delete;
    explicit Value(LuaStatePtrWeak &&ptr, LuaRef ref) noexcept;
    explicit Value(const LuaStatePtrWeak &ptr, LuaRef ref);

    //! Returns status of type check. Type checks assigned to input T.
    //! @tparam T Type to check stored value for
    //! @return If stored value is type of input t
    template <typename T> bool Is() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(rawState(), *ref);
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

    //! Returns status of type check. Type checks assigned to Type enum
    //! @return If stored value is type of input t
    template <Type t> bool Is() {
        const PushLuaValue p(rawState(), *ref);
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

    //! Converts stored lua value to type T and returns the copy
    //! @tparam T Type to convet to
    //! @return Copy of converted value
    template <typename T> T As() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(rawState(), *ref);
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

  public: // Direct lua-bindings
    //! Returns type of lua variable stored in this class. For each
    //! native lua type exists copy inside Type enum
    //! @return Type of this lua variable
    Type LuaType();

    //! Does the same as LuaType() but returns c-string of type
    //! @return Lua type in c-string format
    std::string_view LuaTypeStr();

    //! Returns table field. Only works, if current class stores
    //! table. If not throw exc::NoFieldInTable() error;
    //! @param name Name to search for inside table
    //! @return Value from table
    Value Field(std::string_view name);

    //! Returns table field. Safe version of field. If current class is
    //! not a table or field is not present in table it returns empty
    //! std::optional<Value>
    //! @param name Name to search for inside table
    //! @return Potential value from table
    std::optional<Value> FieldMaybe(std::string_view name);

    std::optional<Value> PathMaybe(std::string_view name);

    //! FIXME: REPAIR
    std::vector<Pair> Items();

    //! Returns length of current table. Works only if class stores table
    //! @return Length of table
    int Length();

  protected:
    LuaState *rawState();

  private:
    LuaStatePtrWeak source;
    LuaRefPtrShared ref;
};

} // namespace nj::lua

#endif
