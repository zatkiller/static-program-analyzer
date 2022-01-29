#include "pkb/ProcedureRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestProcedureRow.cpp "

/**
 * 
 */
TEST_CASE("ProcedureRow operator testing") {
    ProcedureRow row1{ "main" };
    ProcedureRow row2{ "main" };
    ProcedureRow row3{ "foo" };

    TEST_LOG << "Test ProcedureRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);

    TEST_LOG << "Test ProcedureRow <";
    REQUIRE(row1 < row3);
    REQUIRE_FALSE(row1 < row1);
    REQUIRE_FALSE(row1 < row2);
}
