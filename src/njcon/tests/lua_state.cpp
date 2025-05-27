#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "njlog.h"
#include "njlua.h"

TEST_CASE("Test catch") { REQUIRE(0 == 0); }

TEST_CASE("Lua State", "[ctor/dctor]") {
    const std::string promt1 = R"lua(
    A = 10
    B = 10.5
    C = nil
    D = "123"
    E = { } 
    F = function () end

    )lua";

    SECTION("global lua func") {
        using namespace nj::lua;
        State state(promt1);
        {
            Value v = state.Global("A");
            REQUIRE(v.As<int>() == 10);
            REQUIRE(v.As<std::string>() == "10");
            REQUIRE(v.LuaType() == Value::Type::Number);
        }
        {
            Value v = state.Global("B");
            REQUIRE(v.As<float>() == 10.5);
            REQUIRE(v.LuaType() == Value::Type::Number);
        }
        {
            Value v = state.Global("C");
            REQUIRE(v.LuaType() == Value::Type::Nil);
        }
        {
            Value v = state.Global("D");
            REQUIRE(v.As<std::string>() == "123");
            REQUIRE(v.LuaType() == Value::Type::String);
        }
        {
            Value v = state.Global("E");
            REQUIRE(v.LuaType() == Value::Type::Table);
        }
        {
            Value v = state.Global("F");
            REQUIRE(v.LuaType() == Value::Type::Function);
        }
    }

    const std::string promt2 = R"lua(
    t = { 
        a = 10,
        b = 20.9,
        c = "123"
    }
    )lua";
    SECTION("TABLE") {
        using namespace nj::lua;
        State state(promt2);
        auto v = state.Global("t");
        REQUIRE(v.LuaType() == Value::Table);
        auto a = v.Field("a");
        REQUIRE(a.LuaType() == Value::Number);
        REQUIRE(a.As<int>() == 10);
        REQUIRE(a.As<float>() == 10.0);
        REQUIRE(a.As<std::string>() == "10");

        // auto items = v.Items();
        // REQUIRE(items[0].first.As<std::string>() == "a");
        // REQUIRE(items[0].second.As<int>() == 10);
        for (auto &[key, val] : v.Items()) {
            std::string k = key.As<std::string>();
            std::string vstr = val.As<std::string>();
            nj::log::Debug("key={} : val={}", k, vstr);
        }
    }
}
