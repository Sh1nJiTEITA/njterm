#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "njlua.h"

TEST_CASE("Test catch") { REQUIRE(0 == 0); }

TEST_CASE("Lua State", "[ctor/dctor]") {
    const std::string promt1 = "local a = 10";

    SECTION("string ctor") {
        nj::lua::State state(promt1);
        // nj::lua::State state(promt1);
    }
}
