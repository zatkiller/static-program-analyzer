#include "PKB/ProcedureRow.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("ProcedureRow getProcName") {
    ProcedureRow row1{ PROC_NAME{"main"} };
    REQUIRE(row1.getProcName() == PROC_NAME{ "main"});

    ProcedureRow row2{ PROC_NAME{""} };
    REQUIRE(row2.getProcName() == PROC_NAME{ "" });
}

TEST_CASE("ProcedureRow ==") {
    ProcedureRow row1{ PROC_NAME{"main"} };
    ProcedureRow row2{ PROC_NAME{"main"} };
    ProcedureRow row3{ PROC_NAME{"foo"} };

    REQUIRE(row1 == row2);
    REQUIRE(row1 == row1);
    REQUIRE_FALSE(row1 == row3);
}
