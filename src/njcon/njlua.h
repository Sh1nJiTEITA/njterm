#pragma once
#ifndef NJ_LUA_H
#define NJ_LUA_H

#include <lua.h>
#include <memory>

namespace nj::lua {

class Value {};

using LuaState = lua_State;
using LuaStateDeleterType = void (*)(LuaState *);
using LuaStatePtrType = std::unique_ptr<LuaState, LuaStateDeleterType>;

class State {
  public:
    State(const std::string &con, bool loadstd = true);
    State(std::string &&con, bool loadstd = true);
    void Exec(const std::string &con);
    void Exec(std::string &&con);

  private:
    LuaStatePtrType state;
};

} // namespace nj::lua

#endif
