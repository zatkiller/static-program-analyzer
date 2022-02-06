#include "PKB/ConstantRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestConstantRow.cpp"

TEST_CASE("ConstantRow operator testing") {
    ConstantRow row1{ CONST{1} };
    ConstantRow row2{ CONST{1} };
    ConstantRow row3{ CONST{2} };

    TEST_LOG << "Test ConstantRow ==";
    REQUIRE(row1 == row2);
    REQUIRE(row1 == row1);

    REQUIRE_FALSE(row1 == row3);
}
