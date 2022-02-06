#include "PKB/VariableRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestVariableRow.cpp "

/**
 *
 */
TEST_CASE("VariableRow operator testing") {
    VariableRow row1{ VAR_NAME{"a"}};
    VariableRow row2{ VAR_NAME{"a"} };
    VariableRow row3{ VAR_NAME{"b"} };

    TEST_LOG << "Test VariableRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}