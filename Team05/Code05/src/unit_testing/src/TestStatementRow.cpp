#include "pkb/StatementRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestStatementRow.cpp "

/**
 *
 */
TEST_CASE("StatementRow operator testing") {
    StatementRow row1{ StatementType::Assignment, 1 };
    StatementRow row2{ StatementType::Assignment, 1 };
    StatementRow row3{ StatementType::Assignment, 2};
    StatementRow row4{ StatementType::While, 1 };
    StatementRow row5{ StatementType::While, 2 };

    TEST_LOG << "Test StatementRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
    REQUIRE_FALSE(row1 == row4);
    REQUIRE_FALSE(row1 == row5);
}