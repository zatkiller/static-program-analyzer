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
    PKBField test1{ PKBType::STATEMENT, true, Content{STMT_LO{1, StatementType::Assignment}} };
    PKBField test2{ PKBType::STATEMENT, true, Content{STMT_LO{2, StatementType::Assignment}} };
    PKBField test3{ PKBType::STATEMENT, true, Content{STMT_LO{1, StatementType::While}} };
    PKBField test4{ PKBType::STATEMENT, true, Content{STMT_LO{2, StatementType::While}} };
    
    TEST_LOG << "Test empty StatementTable#contains";
    REQUIRE_FALSE(table.contains(test1));

    TEST_LOG << "Test duplicate StatementTable#insert";
    table.insert(test1);
    table.insert(test1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty StatementTable#contains";
    REQUIRE(table.contains(test1));
    REQUIRE_FALSE(table.contains(test2));
    REQUIRE_FALSE(table.contains(test3));
    REQUIRE_FALSE(table.contains(test4));
}