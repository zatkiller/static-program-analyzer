#include "PKB/VariableRow.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("VariableRow getStmt") {
    VariableRow row1{ VAR_NAME{"a"} };
    REQUIRE(row1.getVarName() == VAR_NAME{ "a" });
}

TEST_CASE("VariableRow ==") {
    VariableRow row1{ VAR_NAME{"a"} };
    VariableRow row2{ VAR_NAME{"a"} };
    VariableRow row3{ VAR_NAME{"b"} };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);
    REQUIRE_FALSE(row1 == row3);
}
