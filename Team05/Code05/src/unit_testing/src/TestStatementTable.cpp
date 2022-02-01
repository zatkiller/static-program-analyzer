#include "pkb/StatementTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestStatementTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("StatementTable testing") {
    StatementTable table{};

    TEST_LOG << "Test empty StatementTable#contains";
    REQUIRE_FALSE(table.contains(StatementType::Assignment, 1));

    TEST_LOG << "Test duplicate StatementTable#insert";
    table.insert(StatementType::Assignment, 1);
    table.insert(StatementType::Assignment, 1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty StatementTable#contains";
    REQUIRE(table.contains(StatementType::Assignment, 1));
    REQUIRE_FALSE(table.contains(StatementType::Assignment, 2));
    REQUIRE_FALSE(table.contains(StatementType::While, 1));
    REQUIRE_FALSE(table.contains(StatementType::While, 2));
}