#pragma once
#ifndef NJ_LUA_STATE_H
#define NJ_LUA_STAET_H

#include "njluautils.h"
#include <lua.hpp>
#include <memory>
#include <vector>

namespace nj::lua {

class Value;

class State {
  public:
    State(const char *con, bool loadstd = true);
    State(const std::string &con, bool loadstd = true);
    State(std::string &&con, bool loadstd = true);

    Value ReturnTable();
    Value Global(const char *name);
    void Exec(const std::string &con);
    void Exec(std::string &&con);
    void Exec(const char *con);

  private:
    LuaStatePtrShared state;
};

} // namespace nj::lua

#endif
