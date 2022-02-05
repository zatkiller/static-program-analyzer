#include "PKB.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "
#include <memory>

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("PKB testing") {
    //PKB pkb{};

   /* StatementType type1 = StatementType::Assignment;
    StatementType type2 = StatementType::While;
    int statementNumber1 = 1;
    int statementNumber2 = 2;

    REQUIRE_FALSE(table.contains(type1, statementNumber1));

    TEST_LOG << "Test duplicate StatementTable#insert";
    table.insert(type1, statementNumber1);
    table.insert(type1, statementNumber1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty StatementTable#contains";
    REQUIRE(table.contains(type1, statementNumber1));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));
    REQUIRE_FALSE(table.contains(type1, statementNumber2));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));*/
}