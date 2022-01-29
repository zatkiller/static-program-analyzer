#include "pkb/VariableRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestVariableRow.cpp "

/**
 *
 */
TEST_CASE("VariableRow operator testing") {
    VariableRow row1{ "a" };
    VariableRow row2{ "a" };
    VariableRow row3{ "b" };

    TEST_LOG << "Test VariableRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}