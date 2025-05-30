#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

// #include "njlog.h"
// #include "njluaexc.h"
// #include "njluastate.h"
// #include "njluavalue.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

TEST_CASE("Raw Lua State", "[ctor/dctor]") {
    const std::string promt1 = R"lua(

    A = 10
    B = 10.5
    C = nil
    D = "123"
    E = { } 
    F = function () end

    )lua";

    SECTION("DO STRING") {
        lua_State *state = luaL_newstate();
        luaL_openlibs(state);

        int res = luaL_dostring(state, promt1.c_str());
        REQUIRE(res == 0);

        int type = lua_getglobal(state, "A");
        REQUIRE(type == LUA_TNUMBER);

        int A_val = lua_tonumber(state, -1);
        REQUIRE(A_val == 10);

        lua_pop(state, 1);
    }
}
