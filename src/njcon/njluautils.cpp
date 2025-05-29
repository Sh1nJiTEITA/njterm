#include "njluautils.h"

namespace nj::lua {
PushLuaValue::PushLuaValue(LuaState *state, int ref)
    : state{state}, isKilled{false} {
    lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
}
PushLuaValue::~PushLuaValue() { lua_pop(state, 1); }

void PushLuaValue::kill() {
    lua_pop(state, 1);
    isKilled = true;
}
} // namespace nj::lua
