#include "PKB/StatementTable.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("StatementTable insert and contains") {
    StatementTable table{};
    StatementType type1 = StatementType::Assignment;
    StatementType type2 = StatementType::While;
    int statementNumber1 = 1;
    int statementNumber2 = 2;

    REQUIRE_FALSE(table.contains(type1, statementNumber1));

    table.insert(type1, statementNumber1);
    table.insert(type1, statementNumber1);
    REQUIRE(table.getSize() == 1);

    REQUIRE(table.contains(type1, statementNumber1));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));
    REQUIRE_FALSE(table.contains(type1, statementNumber2));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));
}

TEST_CASE("StatementTable getSize") {
    StatementTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert(StatementType::Assignment, 1);
    REQUIRE(table.getSize() == 1);

    table.insert(StatementType::Assignment, 1);
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("StatementTable getAllStmt") {
    StatementTable table{};
    std::vector<STMT_LO> expected{};
    REQUIRE(expected == table.getAllStmt());

    table.insert(StatementType::Assignment, 2);
    table.insert(StatementType::Assignment, 1);
    expected = { STMT_LO{1, StatementType::Assignment}, STMT_LO{2, StatementType::Assignment} };
    REQUIRE(expected == table.getAllStmt());
}
