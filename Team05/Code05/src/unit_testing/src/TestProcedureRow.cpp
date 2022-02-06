#include "PKB/ProcedureRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestProcedureRow.cpp "

/**
 * 
 */
TEST_CASE("ProcedureRow operator testing") {
    ProcedureRow row1{ PROC_NAME{"main"}};
    ProcedureRow row2{ PROC_NAME{"main"} };
    ProcedureRow row3{ PROC_NAME{"foo"} };

    TEST_LOG << "Test ProcedureRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}