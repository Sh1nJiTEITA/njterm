#include "njlog.h"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <njcon.h>
#include <njluavalue.h>
//
//
//

TEST_CASE("Config test", "[config-class]") {
    using namespace nj::con;
    Config conf{};
    auto v = conf.Get("test.field");
    REQUIRE(v.As<int>() == 1488);
}

TEST_CASE("Instance Config test", "[instance-config-functions]") {
    using namespace nj::con;
    auto v = nj::con::Get("test.field");
    REQUIRE(v.As<int>() == 1488);
}

TEST_CASE("Loading config", "[config-class]") {
    using namespace nj::con;

    std::string promt = R"lua( 
        return { 
            test = { 
                some_field = "123",
                some_other_field = 99.99,
            }
        }
    )lua";

    Load(std::move(promt));
    REQUIRE(Get("test.field").As<int>() == 1488);
    REQUIRE(Get("test.some_field").As<std::string>() == "123");
    REQUIRE(Get("test.some_other_field").As<double>() == 99.99);
}
