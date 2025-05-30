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
    using IPair = std::pair<int, Value>;

  public:
    enum Type : int {
        Nil = LUA_TNIL,
        Boolean = LUA_TBOOLEAN,
        LightUserData = LUA_TLIGHTUSERDATA,
        Number = LUA_TNUMBER,
        String = LUA_TSTRING,
        Table = LUA_TTABLE,
        Function = LUA_TFUNCTION,
        UserData = LUA_TUSERDATA,
        Thread = LUA_TTHREAD,
        None = LUA_TNONE,
    };

    Value() = delete;
    explicit Value(LuaStatePtrWeak &&ptr, LuaRef ref) noexcept;
    explicit Value(const LuaStatePtrWeak &ptr, LuaRef ref);

    //! Returns status of type check. Type checks assigned to input T.
    //! @tparam T Type to check stored value for
    //! @return If stored value is type of input t
    template <typename T> bool Is() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(RawState(), *ref);
        bool result = false;
        if constexpr (meta::IsIntOnly<_T>::value) {
            result = lua_isinteger(RawState(), -1);
        } else if constexpr (meta::IsBool<_T>::value) {
            result = lua_isboolean(RawState(), -1);
        } else if constexpr (meta::IsReal<_T>::value) {
            result = lua_isnumber(RawState(), -1);
        } else if constexpr (meta::IsString<_T>::value) {
            result = lua_isstring(RawState(), -1);
        }
        return result;
    }

    //! Returns status of type check. Type checks assigned to Type enum
    //! @return If stored value is type of input t
    template <Type t> bool Is() {
        const PushLuaValue p(RawState(), *ref);
        bool result = false;
        if constexpr (t == Type::Nil) {
            result = lua_isnil(RawState(), -1);
        } else if constexpr (t == Type::Boolean) {
            result = lua_isboolean(RawState(), -1);
        } else if constexpr (t == Type::LightUserData) {
            result = lua_islightuserdata(RawState(), -1);
        } else if constexpr (t == Type::Number) {
            result = lua_isnumber(RawState(), -1);
        } else if constexpr (t == Type::String) {
            result = lua_isstring(RawState(), -1);
        } else if constexpr (t == Type::Table) {
            result = lua_istable(RawState(), -1);
        } else if constexpr (t == Type::Function) {
            result = lua_isfunction(RawState(), -1);
        } else if constexpr (t == Type::UserData) {
            result = lua_isuserdata(RawState(), -1);
        } else if constexpr (t == Type::Thread) {
            result = lua_isthread(RawState(), -1);
        } else if constexpr (t == Type::None) {
            result = lua_isnone(RawState(), -1);
        }
        return result;
    }

    //! Converts stored lua value to type T and returns the copy
    //! @tparam T Type to convet to
    //! @return Copy of converted value
    template <typename T> T As() {
        using _T = std::decay_t<T>;
        const PushLuaValue p(RawState(), *ref);
        T result{};
        if constexpr (meta::IsIntOnly<_T>::value) {
            result = lua_tointeger(RawState(), -1);
        } else if constexpr (meta::IsBool<_T>::value) {
            result = lua_toboolean(RawState(), -1);
        } else if constexpr (meta::IsReal<_T>::value) {
            result = lua_tonumber(RawState(), -1);
        } else if constexpr (meta::IsString<_T>::value) {
            result = lua_tostring(RawState(), -1);
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

    //! Returns table value with specified path.
    //! Path must be separted with '.' like 'A.B.C' which means: find in table A
    //! in table B field C
    //! @param name Path through tables to field
    std::optional<Value> PathMaybe(std::string_view name);

    //! Returns all key-value pairs from the table.
    //! Each pair is returned as a Value::Pair which consists of key and value.
    //! Only works if current class stores a table. Otherwise, returns empty
    //! vector.
    //! @return Vector of key-value pairs from table
    std::vector<Pair> Pairs();

    //! Returns all values from array-like part of the table using Lua-style
    //! indexing (1-based). Values are returned with their integer index as a
    //! pair (index, value). Iteration stops at first missing (nil) index.
    //! @return Vector of index-value pairs from table
    std::vector<IPair> IPairs();

    //! Returns all keys from the table.
    //! Keys are returned as vector of Value, representing each key in table.
    //! Only works if current class stores a table. Otherwise, returns empty
    //! vector.
    //! @return Vector of keys from table
    std::vector<Value> Keys();

    //! Returns length of current table. Works only if class stores table
    //! @return Length of table
    int Length();

  protected:
    LuaState *RawState();

  private:
    LuaStatePtrWeak source;
    LuaRefPtrShared ref;
};

} // namespace nj::lua

#endif
