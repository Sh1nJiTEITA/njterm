#pragma once
#ifndef NJ_LUA_STATE_H
#define NJ_LUA_STAET_H

#include "njluautils.h"

namespace nj::lua {

class Value;

class State {
  public:
    State(std::string_view con, bool loadstd = true);
    State(const std::string &con, bool loadstd = true);
    State(std::string &&con, bool loadstd = true);

    Value ReturnTable();
    Value Global(std::string_view name);
    void Exec(const std::string &con);
    void Exec(std::string &&con);
    void Exec(std::string_view con);
    void ResetExec(const std::string &con);
    void ResetExec(std::string &&con);
    void ResetExec(std::string_view con);

  private:
    LuaRef tableRef;
    LuaStatePtrShared state;
};

} // namespace nj::lua

#endif
