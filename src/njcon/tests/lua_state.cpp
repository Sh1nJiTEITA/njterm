#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#include "njlog.h"
#include "njluaexc.h"
#include "njluastate.h"
#include "njluavalue.h"
//
// TEST_CASE("Test catch") { REQUIRE(0 == 0); }
//
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

            Value vv = v;
            REQUIRE(vv.As<int>() == 10);
            REQUIRE(vv.As<std::string>() == "10");
            REQUIRE(vv.LuaType() == Value::Type::Number);
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
        c = "123",
        d = {
            a = 10,
            b = 30
        }
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

        auto d = v.Field("d");
        REQUIRE(d.Field("a").As<int>() == 10);
        REQUIRE(d.Field("b").As<int>() == 30);
    }
    //
    const std::string promt3 = R"lua(
    return {
        a = 10,
        b = 20.9,
        c = "123",
        d = {
            a = 10,
            b = 30, 
            d = { 
                a = 20,
            }
        }
    }
    )lua";
    SECTION("RETURN TABLE") {
        using namespace nj::lua;
        State state(promt3);
        auto v = state.ReturnTable();
        REQUIRE(v.Field("a").As<int>() == 10);
        auto f = v.FieldMaybe("a");
        REQUIRE(f.has_value());
        REQUIRE(f.value().As<int>() == 10);

        auto f2 = v.FieldMaybe("ddd");
        REQUIRE(!f2.has_value());

        try {
            State state(promt2);
            auto v = state.ReturnTable();
        } catch (const exc::NoReturnTable &e) {
            REQUIRE(true);
        }

        const std::string promt3_1 = R"lua(
            return 10;
        )lua";

        try {
            State state(promt3_1);
            auto v = state.ReturnTable();
        } catch (const exc::NoReturnTable &e) {
            REQUIRE(true);
        }
    }
    SECTION("PathMaybe") {
        using namespace nj::lua;
        State state(promt3);
        auto table = state.ReturnTable();
        auto d = table.FieldMaybe("d");
        REQUIRE(d.has_value());
        REQUIRE(d.value().Is<Value::Type::Table>());

        auto f = table.PathMaybe("a");
        REQUIRE(f.has_value());
        REQUIRE(f.value().As<int>() == 10);

        auto keys = table.Keys();
        REQUIRE(!keys.empty());
        // for (auto &key : keys) {
        //     nj::log::Debug("key=\"{}\"", key.As<std::string>());
        // }

        auto f2 = table.PathMaybe("d.b");
        REQUIRE(f2.has_value());
        REQUIRE(f2.value().As<int>() == 30);

        auto f3 = table.PathMaybe("d.d.a");
        REQUIRE(f3.has_value());
        REQUIRE(f3.value().As<int>() == 20);
    }

    SECTION("Keys") {
        const std::string promt = R"lua(
            return {
                a = 1,
                b = 2,
                c = 3,
                d = 4,
                e = 5
            }
        )lua";
        using namespace nj::lua;
        State state(promt);
        auto table = state.ReturnTable();
        auto keys = table.Keys();
        std::set<std::string> k;
        for (auto key : keys) {
            k.insert(key.As<std::string>());
        }
        REQUIRE(k.size() == 5);
        REQUIRE(k.contains("a"));
        REQUIRE(k.contains("b"));
        REQUIRE(k.contains("c"));
        REQUIRE(k.contains("d"));
        REQUIRE(k.contains("e"));
    }

    SECTION("Pairs") {
        const std::string promt = R"lua(
            return {
                a = 1,
                b = 2,
                c = 3,
                d = 4,
                e = 5,
            }
        )lua";

        using namespace nj::lua;
        State state(promt);
        auto table = state.ReturnTable();
        auto pairs = table.Pairs();

        std::set<std::string> k;
        std::set<std::string> v;
        for (auto &[key, val] : pairs) {
            k.insert(key.As<std::string>());
            v.insert(val.As<std::string>());
        }
        REQUIRE(k.size() == 5);
        REQUIRE(v.size() == 5);

        REQUIRE(k.contains("a"));
        REQUIRE(k.contains("b"));
        REQUIRE(k.contains("c"));
        REQUIRE(k.contains("d"));
        REQUIRE(k.contains("e"));

        REQUIRE(v.contains("1"));
        REQUIRE(v.contains("2"));
        REQUIRE(v.contains("3"));
        REQUIRE(v.contains("4"));
        REQUIRE(v.contains("5"));
    }

    SECTION("Pairs on array") {
        const std::string promt = R"lua(
            return {
                 1,
                 2,
                 3,
                 4,
                 5,
            }
        )lua";

        using namespace nj::lua;
        State state(promt);
        auto table = state.ReturnTable();
        auto pairs = table.Pairs();

        std::set<std::string> k;
        std::set<std::string> v;
        for (auto &[key, val] : pairs) {
            k.insert(key.As<std::string>());
            v.insert(val.As<std::string>());
        }
        REQUIRE(k.size() == 5);
        REQUIRE(v.size() == 5);

        REQUIRE(k.contains("1"));
        REQUIRE(k.contains("2"));
        REQUIRE(k.contains("3"));
        REQUIRE(k.contains("4"));
        REQUIRE(k.contains("5"));

        REQUIRE(v.contains("1"));
        REQUIRE(v.contains("2"));
        REQUIRE(v.contains("3"));
        REQUIRE(v.contains("4"));
        REQUIRE(v.contains("5"));
    }

    SECTION("IPairs") {
        const std::string promt = R"lua(
            return {
                 1,
                 2,
                 3,
                 4,
                 5,
            }
        )lua";

        using namespace nj::lua;
        State state(promt);
        auto table = state.ReturnTable();
        auto pairs = table.IPairs();

        std::set<int> k;
        std::set<std::string> v;
        for (auto &[i, val] : pairs) {
            k.insert(i);
            v.insert(val.As<std::string>());
        }
        REQUIRE(k.size() == 5);
        REQUIRE(v.size() == 5);

        REQUIRE(k.contains(1));
        REQUIRE(k.contains(2));
        REQUIRE(k.contains(3));
        REQUIRE(k.contains(4));
        REQUIRE(k.contains(5));

        REQUIRE(v.contains("1"));
        REQUIRE(v.contains("2"));
        REQUIRE(v.contains("3"));
        REQUIRE(v.contains("4"));
        REQUIRE(v.contains("5"));
    }

    SECTION("As std::vector") {
        const std::string promt = R"lua(
            return {
                 1,
                 2,
                 3,
                 4,
                 5,
            }
        )lua";

        using namespace nj::lua;
        State state(promt);
        auto table = state.ReturnTable();
        auto vec = table.As<std::vector<int>>();

        REQUIRE(vec.size() == 5);

        for (auto val : vec) {
            nj::log::Debug("elem={}", val);
        }

        std::set<int> set{vec.begin(), vec.end()};

        REQUIRE(set.contains(1));
        REQUIRE(set.contains(2));
        REQUIRE(set.contains(3));
        REQUIRE(set.contains(4));
        REQUIRE(set.contains(5));
    }
}
